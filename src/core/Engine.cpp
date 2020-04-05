
#include "Engine.h"

#include "debug/Logger.h"
#include "debug/MyAssert.h"
#include "Platform.h"

#include <iostream>
#include <SDL2/SDL.h>

Engine *Engine::s_engine = NULL;

Engine::Engine()
		: m_state(EngineState::NOT_STARTED), m_initialised(false)
{
}

Engine::~Engine()
{
	destroy();
}

Engine *Engine::getInstance()
{
	if (!s_engine)
	{
		s_engine = new Engine();
	}

	return s_engine;
}

bool Engine::destroy()
{
	if (m_initialised)
	{
		// Destroy all managers
		Platform::destroy();

		LOG(INFO, "Exitting engine, destroying");
		Logger::destroy();

		m_initialised = false;
	}

	return true;
}

bool Engine::init()
{
	if (m_initialised)
		return false;

	if (!Logger::init("Log.txt", ALL))
		return false;

	if (!Platform::init())
		return false;

	LOG(INFO, "Engine initialised");
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

	while (m_state == EngineState::RUNNING || m_state == EngineState::PAUSED)
	{
		while (updateTime > 1 / 60.0f)
		{
			update(updateTime);

			updateTime -= 1 / 60.0f;
		}

		render();

		currentTime = Platform::getHighResTime();
		updateTime += currentTime - lastTime;
		lastTime = currentTime;
	}
}

void Engine::update(float updateTime)
{
	// emit onUpdate();
}

void Engine::render()
{
	// emit render
}

void Engine::stop()
{
	m_state = EngineState::EXITED;
}

bool Engine::registerManager(IManager *manager)
{
	auto handler = manager->getHandler();
	ASSERT(m_managers.find(handler) == m_managers.end(), "Manager already registered");

	m_managers[handler] = std::shared_ptr<IManager>(manager);

	return true;
}

std::weak_ptr<IManager> Engine::getManager(IManagerHandler handler)
{
	auto manager = m_managers[handler];
	ASSERT(manager, "Manager is not registered. " << handler);

	return std::weak_ptr<IManager>(manager);
}