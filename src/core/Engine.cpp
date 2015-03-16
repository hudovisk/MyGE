
#include "core/Engine.h"

#include "debug/Logger.h"
#include "core/Platform.h"

#include <iostream>

EventManager Engine::g_eventManager;
RenderManager Engine::g_renderManager;
DebugRenderManager Engine::g_debugRenderManager;
EntityManager Engine::g_entityManager;
ResourceManager Engine::g_resourceManager;

Engine::Engine()
	: m_state(EngineState::NOT_STARTED), m_initialised(false),
	 m_drawGBuffer(false)
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
		LOG(INFO, "Exitting engine, destroying");
		g_entityManager.destroy();
		g_resourceManager.destroy();
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

	if(!g_renderManager.init(800,800))
		return false;

	if(!g_debugRenderManager.init())
		return false;

	if(!g_resourceManager.init())
		return false;

	if(!g_entityManager.init())
		return false;

	if(!m_inputContext.init("res/scripts/engineInput.xml"))
		return false;

	LOG(INFO, "Engine initialised");

	WindowClosedEventData event;
	g_eventManager.addListenner(EventListenerDelegate::from_method<Engine,
		&Engine::onWindowClosed>(this),	event.getType());

	m_updateStageEvent = std::make_shared<UpdateStageEventData>(0);
	m_preRenderStageEvent = std::make_shared<PreRenderStageEventData>(0);
	m_render1stStageEvent = std::make_shared<Render1stStageEventData>(0);
	m_render2ndStageEvent = std::make_shared<Render2ndStageEventData>(0);
	m_postRenderStageEvent = std::make_shared<PostRenderStageEventData>(0);

	m_initialised = true;

	return m_initialised;
}

void Engine::start()
{
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

	std::vector<InputParsed> inputs = m_inputContext.parse(g_eventManager.getInputQueue());
	for(unsigned int i = 0; i < inputs.size(); i++)
	{
		switch(inputs[i].id)
		{
			case TOGGLE_DRAW_GBUFFER:
				m_drawGBuffer = !m_drawGBuffer;
				break;
			case TOGGLE_DRAW_FPS:
				g_debugRenderManager.renderFps(!g_debugRenderManager.isRenderFps());
				break;
			case TOGGLE_PAUSE:
				if(m_state == EngineState::RUNNING)
				{
					g_renderManager.onEnginePaused();
					m_state = EngineState::PAUSED;	
				} 
				else if(m_state == EngineState::PAUSED)
				{
					g_renderManager.onEngineResumed();
					m_state = EngineState::RUNNING;
				} 
				break;
			case EXIT:
				LOG(INFO, "Exitting engine, pressed ESC");
				m_state = EngineState::EXITED;
				break;
		}
	}

	if(m_state == EngineState::RUNNING)
		g_eventManager.triggerEvent(m_updateStageEvent);
}

void Engine::render()
{
	g_eventManager.triggerEvent(m_preRenderStageEvent);
	
	//Geometric pass
	// g_renderManager.preRender1st();
	g_eventManager.triggerEvent(m_render1stStageEvent);

	//Light pass
	// g_renderManager.preRender2nd();
	if(m_drawGBuffer)
	{
		g_renderManager.renderGBuffer();	
	}
	else
	{
		g_eventManager.triggerEvent(m_render2ndStageEvent);
	}

	g_debugRenderManager.render();

	g_renderManager.postRender();
	g_eventManager.triggerEvent(m_postRenderStageEvent);
}

void Engine::onWindowClosed(IEventDataPtr event)
{
	m_state = EngineState::EXITED;
}

