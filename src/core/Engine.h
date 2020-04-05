#pragma once

#include "IManager.h"

#include <map>

/**
 * @brief EngineState enum
 * @details Represents the possible states the engine can be.
 */
enum class EngineState
{
	NOT_STARTED,

	RUNNING,
	PAUSED,
	EXITED,
};

/**
 * @brief Engine class
 * @details The core of the game engine.
 */
class Engine
{
public:
	static Engine *getInstance();

	/**
 	* Destructor.
 	* Default destructor
 	*/
	~Engine();

	/**
	* Initialization method.
	* initialise all sub systems in the correct order. Should be called only once.
	* @return true in case of successful initialisation, false otherwise.
	*/
	bool init();

	/**
	* Cleanup method.
	* destroy all sub systems in the correct order. Should be called only once.
	* @return true in case of successful cleanup, false otherwise.
	*/
	bool destroy();

	/**
	* Start (main loop) method.
	* Responsible for updating and drawing the objects, should be called only once.
	*/
	void start();

	/**
	* Stop (main loop) method.
	*/
	void stop();

	bool registerManager(IManager *manager);

	std::weak_ptr<IManager> getManager(IManagerHandler handler);

	EngineState getCurrentState();

private:
	/**
		* Constructor.
		* Default constructor
		*/
	Engine();

	void update(float updateTime);
	void render();

	static Engine *s_engine;

	EngineState m_state;

	bool m_initialised;

	std::map<IManagerHandler, std::shared_ptr<IManager>> m_managers;
};