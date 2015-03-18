
#include "systems/SkyboxSystem.h"

#include "core/Engine.h"

SkyboxSystem::SkyboxSystem()
	: m_initialised(false)
{

}
	
SkyboxSystem::~SkyboxSystem()
{
	destroy();
}

bool SkyboxSystem::init()
{
	if(!m_componentPool.init(10))
	{
		LOG(ERROR, "Couldnt initialise m_componentPool with 10 objects.");
		return false;
	}

	if(!m_shader.init("res/shaders/skyboxVertex.glsl", 
		"res/shaders/skyboxFragment.glsl"))
	{
		LOG(ERROR, "Default skybox shader could not initialise.");
		return false;
	}

	LightPassEventData event;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<SkyboxSystem,&SkyboxSystem::render>(this),
		event.getType());

	std::vector<Geometric*> geometrics;
	Engine::g_resourceManager.loadOBJFile("res/models/cube.obj", geometrics, ResourceManager::LOAD_POSITIONS);

	m_boxGeometric = geometrics[0];

	m_initialised = true;
	return true;
}

bool SkyboxSystem::destroy()
{
	if(m_initialised)
	{
		LOG(INFO, "Destroying SkyboxSystem.");
		LightPassEventData event;
		Engine::g_eventManager.removeListenner(
			EventListenerDelegate::from_method<SkyboxSystem,&SkyboxSystem::render>(this),
			event.getType());

		m_componentPool.destroy();
		
		m_initialised = false;
	}

	return true;
}

void SkyboxSystem::render(IEventDataPtr e)
{
	Engine::g_renderManager.bindSkyboxPass();

	Engine::g_renderManager.bindShader(m_shader);

	// const Matrix4 invCamera;
	const Matrix4& invCamera = 
		Engine::g_renderManager.getDefaultCameraTransform()->getInverseMatrix();
	const Matrix4& projection = Engine::g_renderManager.getDefaultCameraProjection();

	SkyboxComponent** components = m_componentPool.getUsedBufferCache();
	unsigned int numComponents = m_componentPool.getUsedSize();
	if(numComponents == 0) return;

	m_shader.setMatrix4f("MVP_Matrix", invCamera * projection);

	Engine::g_renderManager.bindCubeMapTexture(0, components[0]->m_cubeMapTexture);
	m_shader.set1i("gCubeMapSampler", 0);

	Engine::g_renderManager.render(m_boxGeometric);
}

Component* SkyboxSystem::create()
{
	SkyboxComponent* component = m_componentPool.create();
	component->m_cubeMapTexture = 0;

	return component;
}
	
Component* SkyboxSystem::createFromJSON(const rapidjson::Value& jsonObject)
{
	SkyboxComponent* component = m_componentPool.create();
	std::vector<std::string> files(6);

	for(auto itMember = jsonObject.MemberBegin();
		itMember != jsonObject.MemberEnd(); itMember++)
	{
		if(strcmp("positiveX", itMember->name.GetString()) == 0)
		{
			files[0] = itMember->value.GetString();
		}
		else if(strcmp("negativeX", itMember->name.GetString()) == 0)
		{
			files[1] = itMember->value.GetString();
		}
		else if(strcmp("positiveY", itMember->name.GetString()) == 0)
		{
			files[2] = itMember->value.GetString();
		}
		else if(strcmp("negativeY", itMember->name.GetString()) == 0)
		{
			files[3] = itMember->value.GetString();
		}
		else if(strcmp("positiveZ", itMember->name.GetString()) == 0)
		{
			files[4] = itMember->value.GetString();
		}
		else if(strcmp("negativeZ", itMember->name.GetString()) == 0)
		{
			files[5] = itMember->value.GetString();
		}
		else if(strcmp("size", itMember->name.GetString()) == 0)
		{
			component->m_size = itMember->value.GetDouble();
		}
	}

	component->m_cubeMapTexture = Engine::g_renderManager.createCubeMapTexFromImgs(files);

	return component;
}

void SkyboxSystem::release(Component* component)
{
	SkyboxComponent* skybox = dynamic_cast<SkyboxComponent*>(component);
	Engine::g_renderManager.releaseCubeMap(skybox->m_cubeMapTexture);
}
