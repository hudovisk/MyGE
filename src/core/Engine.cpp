
#include "core/Engine.h"

#include "debug/Logger.h"
#include "core/System.h"

#include <iostream>

EventManager Engine::g_eventManager;
RenderManager Engine::g_renderManager;
DebugRenderManager Engine::g_debugRenderManager;

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
	g_debugRenderManager.destroy();
	g_renderManager.destroy();
	g_eventManager.destroy();
	System::destroy();
	Logger::destroy();

	return true;
}

bool Engine::init()
{
	if(m_initialised)
		return false;

	if(!Logger::init("Log.txt", ALL))
		return false;

	if(!System::init())
		return false;

	if(!g_eventManager.init())
		return false;

	if(!g_renderManager.init(800,600))
		return false;

	if(!g_debugRenderManager.init())
		return false;

	LOG(INFO, "Engine initialised");

	g_eventManager.addListenner(EventListenerDelegate::from_method<Engine,&Engine::onWindowClosed>(this),
															(new WindowClosedEventData)->getType());

	m_initialised = true;

	return m_initialised;
}

void Engine::start()
{
	float lastTime = System::getHighResTime();
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
			std::cout<<"Frames/s : "<<framesCounter / frameCounterTime<<std::endl;
			framesCounter = 0;
			frameCounterTime--;
		}

		currentTime = System::getHighResTime();
		frameCounterTime += currentTime - lastTime;
		updateTime += currentTime - lastTime;
		lastTime = currentTime;
	}
}

void Engine::update(float updateTime)
{
	
}

void Engine::render()
{
	g_renderManager.preRender();

	g_renderManager.render();
	g_debugRenderManager.render();

	g_renderManager.postRender();
}

void Engine::onWindowClosed(IEventDataPtr event)
{
	m_state = EngineState::EXITED;
}

