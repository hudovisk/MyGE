
#include "systems/LightSystem.h"

#include "core/Engine.h"
#include "debug/MyAssert.h"

#include "assimp-3.1.1/Importer.hpp"
#include "assimp-3.1.1/postprocess.h"
#include "assimp-3.1.1/scene.h"

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

	Render2ndStageEventData event;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<LightSystem,&LightSystem::render>(this),
		event.getType());

	m_isInitialised = true;
	return true;
}

bool LightSystem::initShaders()
{
	if(!m_lightShaders[LightComponent::POINT_TYPE].init("res/shaders/lightVertex.glsl", 
		"res/shaders/pointLightFragment.glsl"))
	{
		LOG(ERROR, "Default point light shader could not initialise.");
		return false;
	}

	if(!m_lightShaders[LightComponent::DIRECTIONAL_TYPE].init("res/shaders/lightVertex.glsl", 
		"res/shaders/dirLightFragment.glsl"))
	{
		LOG(ERROR, "Default directional light shader could not initialise.");
		return false;
	}
	if(!m_lightShaders[LightComponent::SPOT_TYPE].init("res/shaders/lightVertex.glsl", 
		"res/shaders/spotLightFragment.glsl"))
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
		Render2ndStageEventData event;
		Engine::g_eventManager.removeListenner(
			EventListenerDelegate::from_method<LightSystem,&LightSystem::render>(this),
			event.getType());
		for(int i=0; i < LightComponent::NUM_TYPES; i++)
		{
			m_componentPools[i].destroy();
			m_lightShaders[i].destroy();
			// Engine::g_renderManager.releaseGeometric(m_lightGeometrics[i]);
		}

		m_isInitialised = false;
	}

	return true;
}

void LightSystem::render(IEventDataPtr e)
{
	Engine::g_renderManager.bindLightPass();

	renderPointLights();

	renderDirectionalLights();

	renderSpotLights();
}

void LightSystem::renderPointLights()
{
	Engine::g_renderManager.bindShader(m_lightShaders[LightComponent::POINT_TYPE]);
	//Set uniform stuff.
	m_lightShaders[LightComponent::POINT_TYPE].set1i("gPositionMap",
		RenderManager::GBUFFER_TEXTURE_TYPE_POSITION);
	m_lightShaders[LightComponent::POINT_TYPE].set1i("gNormalMap",
		RenderManager::GBUFFER_TEXTURE_TYPE_NORMAL);
	m_lightShaders[LightComponent::POINT_TYPE].set1i("gColorMap", 
		RenderManager::GBUFFER_TEXTURE_TYPE_DIFFUSE);

	const Matrix4 projection;

	float width = Engine::g_renderManager.getWidth();
	float height = Engine::g_renderManager.getHeight();
	m_lightShaders[LightComponent::POINT_TYPE].set2f("gScreenSize",
				 width, height);

	const Vec3& cameraPos = Engine::g_renderManager.getDefaultCameraTransform()->getPosition();
	m_lightShaders[LightComponent::POINT_TYPE].setVec3f("gEyeWorldPos",
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
			m_lightShaders[LightComponent::POINT_TYPE].setMatrix4f("MVP_Matrix",
				 /*modelView **/ projection);
			m_lightShaders[LightComponent::POINT_TYPE].setVec3f("gLight.pos",
				 message.getTransform()->getPosition());
			m_lightShaders[LightComponent::POINT_TYPE].setVec3f("gLight.color",
				 component->m_color);
			m_lightShaders[LightComponent::POINT_TYPE].set1f("gLight.ambIntensity",
				component->m_ambIntensity);
			m_lightShaders[LightComponent::POINT_TYPE].set1f("gLight.difIntensity",
				component->m_difIntensity);
			m_lightShaders[LightComponent::POINT_TYPE].set1f("gLight.att.expo",
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
	Engine::g_renderManager.bindShader(m_lightShaders[LightComponent::DIRECTIONAL_TYPE]);

	//Set uniform stuff.
	m_lightShaders[LightComponent::DIRECTIONAL_TYPE].set1i("gPositionMap",
		RenderManager::GBUFFER_TEXTURE_TYPE_POSITION);
	m_lightShaders[LightComponent::DIRECTIONAL_TYPE].set1i("gNormalMap",
		RenderManager::GBUFFER_TEXTURE_TYPE_NORMAL);
	m_lightShaders[LightComponent::DIRECTIONAL_TYPE].set1i("gColorMap", 
		RenderManager::GBUFFER_TEXTURE_TYPE_DIFFUSE);

	//identity projection
	Matrix4 projection;
	projection.identity();
	m_lightShaders[LightComponent::DIRECTIONAL_TYPE].setMatrix4f("MVP_Matrix",
				 projection);

	float width = Engine::g_renderManager.getWidth();
	float height = Engine::g_renderManager.getHeight();
	m_lightShaders[LightComponent::DIRECTIONAL_TYPE].set2f("gScreenSize",
				 width, height);

	const Vec3& cameraPos = Engine::g_renderManager.getDefaultCameraTransform()->getPosition();
	m_lightShaders[LightComponent::DIRECTIONAL_TYPE].setVec3f("gEyeWorldPos",
				 cameraPos);

	//Get all directional lights
	LightComponent** directionalLights =  
		m_componentPools[LightComponent::DIRECTIONAL_TYPE].getUsedBufferCache();
	unsigned int numDirectionalLights = 
		m_componentPools[LightComponent::DIRECTIONAL_TYPE].getUsedSize();
	for(unsigned int i = 0; i < numDirectionalLights; i++)
	{
		LightComponent* component = directionalLights[i];

		m_lightShaders[LightComponent::DIRECTIONAL_TYPE].setVec3f("gLight.direction",
			 component->m_direction);
		m_lightShaders[LightComponent::DIRECTIONAL_TYPE].setVec3f("gLight.color",
			 component->m_color);
		m_lightShaders[LightComponent::DIRECTIONAL_TYPE].set1f("gLight.ambIntensity",
			component->m_ambIntensity);
		m_lightShaders[LightComponent::DIRECTIONAL_TYPE].set1f("gLight.difIntensity",
			component->m_difIntensity);

		//Finally render it.
		Engine::g_renderManager.render(m_lightGeometrics[LightComponent::DIRECTIONAL_TYPE]);
	}

}

void LightSystem::renderSpotLights()
{
	Engine::g_renderManager.bindShader(m_lightShaders[LightComponent::SPOT_TYPE]);
	//Set uniform stuff.
	m_lightShaders[LightComponent::SPOT_TYPE].set1i("gPositionMap",
		RenderManager::GBUFFER_TEXTURE_TYPE_POSITION);
	m_lightShaders[LightComponent::SPOT_TYPE].set1i("gNormalMap",
		RenderManager::GBUFFER_TEXTURE_TYPE_NORMAL);
	m_lightShaders[LightComponent::SPOT_TYPE].set1i("gColorMap", 
		RenderManager::GBUFFER_TEXTURE_TYPE_DIFFUSE);

	const Matrix4 projection;

	float width = Engine::g_renderManager.getWidth();
	float height = Engine::g_renderManager.getHeight();
	m_lightShaders[LightComponent::SPOT_TYPE].set2f("gScreenSize",
				 width, height);

	const Vec3& cameraPos = Engine::g_renderManager.getDefaultCameraTransform()->getPosition();
	m_lightShaders[LightComponent::SPOT_TYPE].setVec3f("gEyeWorldPos",
				 cameraPos);

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
			Matrix4& model = message.getTransform()->getMatrix();
			Vec3 direction(-model.m_data[8], -model.m_data[9], -model.m_data[10]);

			//More uniform stuff.
			m_lightShaders[LightComponent::SPOT_TYPE].setMatrix4f("MVP_Matrix",
				 /*modelView **/ projection);
			m_lightShaders[LightComponent::SPOT_TYPE].setVec3f("gLight.pos",
				 message.getTransform()->getPosition());
			m_lightShaders[LightComponent::SPOT_TYPE].setVec3f("gLight.direction",
				 direction);
			m_lightShaders[LightComponent::SPOT_TYPE].setVec3f("gLight.color",
				 component->m_color);
			m_lightShaders[LightComponent::SPOT_TYPE].set1f("gLight.minSpotFactor",
				component->m_minSpotFactor);
			m_lightShaders[LightComponent::SPOT_TYPE].set1f("gLight.ambIntensity",
				component->m_ambIntensity);
			m_lightShaders[LightComponent::SPOT_TYPE].set1f("gLight.difIntensity",
				component->m_difIntensity);
			m_lightShaders[LightComponent::SPOT_TYPE].set1f("gLight.att.expo",
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
