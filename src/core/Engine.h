#ifndef ENGINE_H
#define ENGINE_H

#include "events/EventManager.h"
#include "render/RenderManager.h"
#include "debug/DebugRenderManager.h"
#include "core/EntityManager.h"
#include "core/ResourceManager.h"

#include "events/InputContext.h"
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

enum class UpdateStage
{
	UPDATE,
	PRE_RENDER,
	RENDER,
	POST_RENDER,
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
	static DebugRenderManager g_debugRenderManager;
	static EntityManager g_entityManager;
	static ResourceManager g_resourceManager;

private:
	void update(float updateTime);
	void render();

	enum INPUT_MESSAGES
	{
		TOGGLE_DRAW_GBUFFER = 1,
		TOGGLE_DRAW_FPS = 2,
		TOGGLE_PAUSE = 3,
		EXIT = 4,
	};

	EngineState m_state;

	std::shared_ptr<UpdateStageEventData> m_updateStageEvent;
	IEventDataPtr m_preRenderStageEvent;
	IEventDataPtr m_geometricPassEvent;
	IEventDataPtr m_lightPassEvent;
	IEventDataPtr m_postRenderStageEvent;

	InputContext m_inputContext;

	bool m_initialised;
	bool m_drawGBuffer;
};

#endif