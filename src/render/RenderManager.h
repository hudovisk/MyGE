
#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include "events/Events.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <vector>

class RenderManager 
{
public:
	/**
	 * @brief Default constructor
	 */
	RenderManager() 
		: m_window(NULL), m_screenSurface(NULL), m_width(0), m_height(0) { } 

	/**
	 * @brief Initialise RenderManager
	 * @details Creates a window of size in pixels defined by the parameters width and height.
	 * Initialises the VIDEO syb system of SDL as well as initialise OpenGL configurations.
	 * It also register onWindowResize call-back in Engine::g_eventManager.
	 * 
	 * @param width width of the window.
	 * @param height height of the window.
	 * 
	 * @return true if success, false otherwise.
	 */
	bool init(int width, int height);

	/**
	 * @brief Cleanup method
	 * @details Quit SDL Video sub systems.
	 * @return true if success, false otherwise.
	 */
	bool destroy();

	void preRender();
	void render();
	void postRender();

	/**
	 * @brief Callback for WindowResize event.
	 *  
	 * @param event event should be cast to WindowResizedEventData where the new width and height can be retrieved
	 */
	void onWindowResize(IEventDataPtr event);

	// ShaderPtr createShader(std::string vertexFilePath, std::string fragmentFilePath);

	int getWidth() { return m_width; }
	int getHeight() { return m_height; }

private:
	bool initSDL();
	bool initGL();

	SDL_Window* m_window;
	SDL_Surface* m_screenSurface;
	SDL_GLContext m_glContext;

	int m_width, m_height;

	//static unsigned int NEXT_RENDER_COMPONENT_ID;
	//std::vector<RenderComponentPtr> m_renderComponents;
};

#endif