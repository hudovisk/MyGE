
#include "core/Engine.h"

#include "debug/Logger.h"
#include "core/Platform.h"

#include <iostream>

EventManager Engine::g_eventManager;
RenderManager Engine::g_renderManager;
DebugRenderManager Engine::g_debugRenderManager;
EntityManager Engine::g_entityManager;

Engine::Engine()
	: m_state(EngineState::NOT_STARTED), m_initialised(false)
{

}

Engine::~Engine()
{
	destroy();
}

bool Engine::destroy()
{  
	if(m_initialised)
	{
		g_entityManager.destroy();
		g_debugRenderManager.destroy();
		g_renderManager.destroy();
		g_eventManager.destroy();
		Platform::destroy();
		Logger::destroy();

		m_initialised = false;
	}	

	return true;
}

bool Engine::init()
{
	if(m_initialised)
		return false;

	if(!Logger::init("Log.txt", ALL))
		return false;

	if(!Platform::init())
		return false;

	if(!g_eventManager.init())
		return false;

	if(!g_renderManager.init(800,600))
		return false;

	if(!g_debugRenderManager.init())
		return false;

	if(!g_entityManager.init())
		return false;

	LOG(INFO, "Engine initialised");

	g_eventManager.addListenner(EventListenerDelegate::from_method<Engine,&Engine::onWindowClosed>(this),
															(new WindowClosedEventData)->getType());

	m_updateStageEvent = std::make_shared<UpdateStageEventData>(0);
	m_preRenderStageEvent = std::make_shared<PreRenderStageEventData>(0);
	m_renderStageEvent = std::make_shared<RenderStageEventData>(0);
	m_postRenderStageEvent = std::make_shared<PostRenderStageEventData>(0);

	m_initialised = true;

	return m_initialised;
}

void Engine::start()
{
	//g_entityManager.loadEntities("res/scripts/entities.json");
	// g_entityManager.loadPlayer("res/models/monkey.obj");
	// for(int i=0; i<99; i++)
	// 	g_entityManager.loadEntity("res/models/monkey.obj");

	float lastTime = Platform::getHighResTime();
	float currentTime = lastTime;
	float updateTime = 0;
	float frameCounterTime = 0;
	int framesCounter = 0;

	m_state = EngineState::RUNNING;

	g_debugRenderManager.renderFps(true);

	while(m_state == EngineState::RUNNING || m_state == EngineState::PAUSED)
	{
		while(updateTime > 1/60.0f)
		{
			update(updateTime);

			g_eventManager.dispatchEvents();

			updateTime-=1/60.0f;	
		}
		
		render();
		framesCounter++;

		if(frameCounterTime > 1.0f)
		{
			g_debugRenderManager.setFps(framesCounter / frameCounterTime);
			//std::cout<<"Frames/s : "<<framesCounter / frameCounterTime<<std::endl;
			framesCounter = 0;
			frameCounterTime--;
		}

		currentTime = Platform::getHighResTime();
		frameCounterTime += currentTime - lastTime;
		updateTime += currentTime - lastTime;
		lastTime = currentTime;
	}
}

void Engine::update(float updateTime)
{
	m_updateStageEvent->setDeltaT(updateTime);
	g_eventManager.triggerEvent(m_updateStageEvent);
}

void Engine::render()
{
	g_eventManager.triggerEvent(m_preRenderStageEvent);
	g_renderManager.preRender();

	g_eventManager.triggerEvent(m_renderStageEvent);
	// g_renderManager.render(meshes);
	g_debugRenderManager.render();

	g_renderManager.postRender();
	g_eventManager.triggerEvent(m_postRenderStageEvent);
}

void Engine::onWindowClosed(IEventDataPtr event)
{
	m_state = EngineState::EXITED;
}

