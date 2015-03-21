
#include "systems/LightSystem.h"

#include "core/Engine.h"
#include "debug/MyAssert.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

extern const float PIdiv180;

/**
 * TODO: FIX THIS HACK (sort of)!
 */
void getVerticesAndIndices(std::vector<Vertex>& vertices, 
	std::vector<unsigned int>& indices, const aiMesh* aMesh);
extern Geometric* loadFile(std::string filePath); 

LightSystem::LightSystem()
	: m_isInitialised(false)
{

}

LightSystem::~LightSystem()
{
	destroy();
}

bool LightSystem::init()
{
	if(m_isInitialised)
		return true;

	for(int i=0; i < LightComponent::NUM_TYPES; i++)
	{
		if(!m_componentPools[i].init(10))
		{
			LOG(ERROR, "Couldnt initialise m_componentPool with 10 objects.");
			return false;
		}
	}

	if(!initShaders())
		return false;

	if(!initGeometrics())
		return false;

	m_shadowStencilEvent = std::make_shared<ShadowStencilPassEventData>(0);

	LightPassEventData event;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<LightSystem,&LightSystem::render>(this),
		event.getType());

	m_isInitialised = true;
	return true;
}

bool LightSystem::initShaders()
{
	if(!m_lpPointShader.init("res/shaders/lightVertex.glsl", "res/shaders/pointLightFragment.glsl"))
	{
		LOG(ERROR, "Default point light shader could not initialise.");
		return false;
	}

	if(!m_lpDirShader.init("res/shaders/lightVertex.glsl", "res/shaders/dirLightFragment.glsl"))
	{
		LOG(ERROR, "Default directional light shader could not initialise.");
		return false;
	}
	if(!m_lpSpotShader.init("res/shaders/lightVertex.glsl", "res/shaders/spotLightFragment.glsl"))
	{
		LOG(ERROR, "Default spot light shader could not initialise.");
		return false;
	}

	return true;
}

bool LightSystem::initGeometrics()
{
	std::vector<Geometric*> geometrics;
	Engine::g_resourceManager.loadOBJFile("res/models/sphere.obj", geometrics, ResourceManager::LOAD_POSITIONS);
	Engine::g_resourceManager.loadOBJFile("res/models/quad.obj", geometrics, ResourceManager::LOAD_POSITIONS);

	m_lightGeometrics[LightComponent::POINT_TYPE] = geometrics[0];
	m_lightGeometrics[LightComponent::DIRECTIONAL_TYPE] = geometrics[1];
	// m_lightGeometrics[LightComponent::SPOT_TYPE] = loadFile("res/models/spot.obj");
	return true;
}

bool LightSystem::destroy()
{
	if(m_isInitialised)
	{
		LOG(INFO, "Destroying LightSystem.");
		LightPassEventData event;
		Engine::g_eventManager.removeListenner(
			EventListenerDelegate::from_method<LightSystem,&LightSystem::render>(this),
			event.getType());
		for(int i=0; i < LightComponent::NUM_TYPES; i++)
		{
			m_componentPools[i].destroy();
			//Engine::g_renderManager.releaseGeometric(m_lightGeometrics[i]);
		}
		
		m_lpPointShader.destroy();
		m_lpSpotShader.destroy();
		m_lpDirShader.destroy();

		m_isInitialised = false;
	}

	return true;
}

void LightSystem::render(IEventDataPtr e)
{
	//Engine::g_renderManager.bindLightPass();

	// renderPointLights();

	renderDirectionalLights();

	renderSpotLights();
}

void LightSystem::renderPointLights()
{
	Engine::g_renderManager.bindShader(m_lpPointShader);
	//Set uniform stuff.
	m_lpPointShader.set1i("gPositionMap",
		RenderManager::GBUFFER_TEXTURE_TYPE_POSITION);
	m_lpPointShader.set1i("gNormalMap",
		RenderManager::GBUFFER_TEXTURE_TYPE_NORMAL);
	m_lpPointShader.set1i("gColorMap", 
		RenderManager::GBUFFER_TEXTURE_TYPE_DIFFUSE);

	const Matrix4 projection;

	float width = Engine::g_renderManager.getWidth();
	float height = Engine::g_renderManager.getHeight();
	m_lpPointShader.set2f("gScreenSize",
				 width, height);

	const Vec3& cameraPos = Engine::g_renderManager.getDefaultCameraTransform()->getPosition();
	m_lpPointShader.setVec3f("gEyeWorldPos",
				 cameraPos);

	GetTransformMessage message;

	//Get all point lights
	LightComponent** pointLights =  
		m_componentPools[LightComponent::POINT_TYPE].getUsedBufferCache();
	unsigned int numPointLights = 
		m_componentPools[LightComponent::POINT_TYPE].getUsedSize();
	for(unsigned int i = 0; i < numPointLights; i++)
	{
		LightComponent* component = pointLights[i];

		message.setEntityHandler(component->m_entity);
		message.setHandled(false);
		Engine::g_entityManager.sendMessage(&message);

		if(message.isHandled())
		{
			//More uniform stuff.
			m_lpPointShader.setMatrix4f("MVP_Matrix",
				 /*modelView **/ projection);
			m_lpPointShader.setVec3f("gLight.pos",
				 message.getTransform()->getPosition());
			m_lpPointShader.setVec3f("gLight.color",
				 component->m_color);
			m_lpPointShader.set1f("gLight.ambIntensity",
				component->m_ambIntensity);
			m_lpPointShader.set1f("gLight.difIntensity",
				component->m_difIntensity);
			m_lpPointShader.set1f("gLight.att.expo",
				component->m_expCoefficient);
			//Finally render it.
			Engine::g_renderManager.render(m_lightGeometrics[LightComponent::DIRECTIONAL_TYPE]);
		}
		else
		{
			LOG(WARN, "Light without spatial component.");
		}
	}	
}

void LightSystem::renderDirectionalLights()
{
	Engine::g_renderManager.bindLightPass();
	Engine::g_renderManager.bindShader(m_lpDirShader);

	//Set uniform stuff.
	m_lpDirShader.setPositionSampler(RenderManager::GBUFFER_TEXTURE_TYPE_POSITION);
	m_lpDirShader.setNormalSampler(RenderManager::GBUFFER_TEXTURE_TYPE_NORMAL);
	m_lpDirShader.setColorSampler(RenderManager::GBUFFER_TEXTURE_TYPE_DIFFUSE);

	//identity projection
	Matrix4 projection;
	projection.identity();
	m_lpDirShader.setWVP(projection);

	float width = Engine::g_renderManager.getWidth();
	float height = Engine::g_renderManager.getHeight();
	m_lpDirShader.setScreenSize(width, height);

	const Vec3& cameraPos = Engine::g_renderManager.getDefaultCameraTransform()->getPosition();
	m_lpDirShader.setEyePos(cameraPos);

	//Get all directional lights
	LightComponent** directionalLights =  
		m_componentPools[LightComponent::DIRECTIONAL_TYPE].getUsedBufferCache();
	unsigned int numDirectionalLights = 
		m_componentPools[LightComponent::DIRECTIONAL_TYPE].getUsedSize();
	for(unsigned int i = 0; i < numDirectionalLights; i++)
	{
		LightComponent* component = directionalLights[i];

		m_lpDirShader.setLightDir(component->m_direction);
		m_lpDirShader.setLightColor(component->m_color);
		m_lpDirShader.setAmbIntensity(component->m_ambIntensity);
		m_lpDirShader.setDiffIntensity(component->m_difIntensity);

		//Finally render it.
		Engine::g_renderManager.render(m_lightGeometrics[LightComponent::DIRECTIONAL_TYPE]);
	}

}

void LightSystem::renderSpotLights()
{
	const Matrix4 projection = Engine::g_renderManager.getDefaultCameraProjection();

	float width = Engine::g_renderManager.getWidth();
	float height = Engine::g_renderManager.getHeight();

	const Vec3& cameraPos = Engine::g_renderManager.getDefaultCameraTransform()->getPosition();

	GetTransformMessage message;

	//Get all spot lights
	LightComponent** spotLights =  
		m_componentPools[LightComponent::SPOT_TYPE].getUsedBufferCache();
	unsigned int numSpotLights = 
		m_componentPools[LightComponent::SPOT_TYPE].getUsedSize();
	for(unsigned int i = 0; i < numSpotLights; i++)
	{
		LightComponent* component = spotLights[i];

		message.setEntityHandler(component->m_entity);
		message.setHandled(false);
		Engine::g_entityManager.sendMessage(&message);

		if(message.isHandled())
		{
			const Matrix4& model = message.getTransform()->getMatrix();
			const Matrix4& invModel = message.getTransform()->getInverseMatrix();

			Vec3 direction(-model.m_data[8], -model.m_data[9], -model.m_data[10]);

			//shadow pass
			m_shadowStencilEvent->m_stencil = false;
			m_shadowStencilEvent->m_view = &invModel;
			m_shadowStencilEvent->m_projection = &projection;
			Engine::g_eventManager.triggerEvent(m_shadowStencilEvent);

			Engine::g_renderManager.bindLightPass();
			Engine::g_renderManager.bindShader(m_lpSpotShader);

			m_lpSpotShader.setPositionSampler(RenderManager::GBUFFER_TEXTURE_TYPE_POSITION);
			m_lpSpotShader.setNormalSampler(RenderManager::GBUFFER_TEXTURE_TYPE_NORMAL);
			m_lpSpotShader.setColorSampler(RenderManager::GBUFFER_TEXTURE_TYPE_DIFFUSE);
			m_lpSpotShader.setShadowSampler(RenderManager::SHADOWMAP_TEXTURE_DEPTH);

			m_lpSpotShader.setEyePos(cameraPos);
			m_lpSpotShader.setScreenSize(width, height);

			const Matrix4 identity;
			m_lpSpotShader.setWVP(identity);

			//More uniform stuff.
			m_lpSpotShader.setWVPLight(invModel * projection);
			m_lpSpotShader.setLightPos(message.getTransform()->getPosition());
			m_lpSpotShader.setLightDir(direction);
			m_lpSpotShader.setLightColor(component->m_color);
			m_lpSpotShader.setMinSpotFactor(component->m_minSpotFactor);
			m_lpSpotShader.setAmbIntensity(component->m_ambIntensity);
			m_lpSpotShader.setDiffIntensity(component->m_difIntensity);
			m_lpSpotShader.setExpoAttenuation(component->m_expCoefficient);

			//Finally render it
			Engine::g_renderManager.render(m_lightGeometrics[LightComponent::DIRECTIONAL_TYPE]);
		}
		else
		{
			LOG(WARN, "Light without spatial component.");
		}
	}	
}

Component* LightSystem::create()
{
	LightComponent* component = m_componentPools[LightComponent::POINT_TYPE].create();
	component->m_type = LightComponent::POINT_TYPE;
	component->m_linCoefficient = 1;
	component->m_expCoefficient = 1;
	component->m_ambIntensity = 0.5;
	component->m_difIntensity = 1;
	component->m_color = Vec3(1.0, 1.0, 1.0); // white

	return component;
}

Component* LightSystem::createFromJSON(const rapidjson::Value& jsonObject)
{
	LightComponent* component;
	
	ASSERT(jsonObject.HasMember("type"), "Missing attribute type in LightSystem.");

	int type = strToLightType(jsonObject["type"].GetString());
	component = m_componentPools[type].create();
	component->m_type = type;

	for(auto itMember = jsonObject.MemberBegin();
		itMember != jsonObject.MemberEnd(); itMember++)
	{
		if(strcmp("radius", itMember->name.GetString()) == 0)
		{
			component->m_radius = itMember->value.GetDouble();
			component->m_expCoefficient = 1/(component->m_radius * component->m_radius * 0.01);
		}
		else if(strcmp("angle", itMember->name.GetString()) == 0)
		{
			component->m_minSpotFactor = cos(itMember->value.GetDouble() * PIdiv180);
		}
		else if(strcmp("color", itMember->name.GetString()) == 0)
		{
			component->m_color.m_data[0] = itMember->value[0u].GetDouble();
			component->m_color.m_data[1] = itMember->value[1].GetDouble();
			component->m_color.m_data[2] = itMember->value[2].GetDouble();
		}
		else if(strcmp("direction", itMember->name.GetString()) == 0)
		{
			component->m_direction.m_data[0] = itMember->value[0u].GetDouble();
			component->m_direction.m_data[1] = itMember->value[1].GetDouble();
			component->m_direction.m_data[2] = itMember->value[2].GetDouble();
		}
		else if(strcmp("ambientIntensity", itMember->name.GetString()) == 0)
		{
			component->m_ambIntensity = itMember->value.GetDouble();
		}
		else if(strcmp("diffuseIntensity", itMember->name.GetString()) == 0)
		{
			component->m_difIntensity = itMember->value.GetDouble();
		}
		else if(strcmp("type", itMember->name.GetString()) != 0)
		{
			LOG(WARN, "Unknown attribute: "<<itMember->name.GetString()
				<<" in LightSystem.");
		}
	}

	return component;
}

void LightSystem::release(Component* component)
{
	LightComponent* lightComponent = dynamic_cast<LightComponent*>(component);
	m_componentPools[lightComponent->m_type].release(lightComponent);
}

int LightSystem::strToLightType(const char* str)
{
	if(strcmp("point", str) == 0)
		return LightComponent::POINT_TYPE;
	else if(strcmp("directional", str) == 0)
		return LightComponent::DIRECTIONAL_TYPE;
	else if(strcmp("spot", str) == 0)
		return LightComponent::SPOT_TYPE;
	else 
	{
		LOG(ERROR, "Light type: "<<str<<" unknown, using point light.");
		return LightComponent::POINT_TYPE;
	}
}
