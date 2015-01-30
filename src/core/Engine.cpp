
#include "core/Engine.h"

#include "debug/Logger.h"
#include "core/System.h"

#include <iostream>

EventManager Engine::g_eventManager;

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

	LOG(INFO, "Engine initialised");

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
	
}

void Engine::onWindowClosed(IEventDataPtr event)
{
	m_state = EngineState::EXITED;
}

