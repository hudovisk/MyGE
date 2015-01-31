#ifndef ENGINE_H
#define ENGINE_H

#include "events/EventManager.h"
#include "render/RenderManager.h"

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
	/**
	* Constructor.
	* Default constructor
	*/
	Engine();

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
	* Window closed event callback.
	*/
	void onWindowClosed(IEventDataPtr event);

	EngineState getCurrentState();

	static EventManager g_eventManager;
	static RenderManager g_renderManager;

private:
	void update(float updateTime);
	void render();

	EngineState m_state;

	bool m_initialised;
};

#endif