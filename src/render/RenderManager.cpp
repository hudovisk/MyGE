
#include "render/RenderManager.h"
#include "debug/Logger.h"
#include "core/Engine.h"

//UI32 RenderManager::NEXT_RENDER_COMPONENT_ID = 1;

bool RenderManager::init(int width, int height)
{
	m_width = width;
	m_height = height;

	if(!initSDL())
		return false;

	if(!initGL())
		return false;

	WindowResizedEventData windowResizedEventData;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method
				<RenderManager,&RenderManager::onWindowResize>(this),
		windowResizedEventData.getType());

	return true;
}


bool RenderManager::initSDL()
{
	if(SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		LOG(ERROR, "SDL init error: " << SDL_GetError());
		return false;
	}

	m_window = SDL_CreateWindow( "My Manager", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
																m_width, m_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	//Get window surface
	m_screenSurface = SDL_GetWindowSurface( m_window );
	//Fill the surface white
	SDL_FillRect( m_screenSurface, NULL, SDL_MapRGB( m_screenSurface->format, 0xFF, 0xFF, 0xFF ) );
	//Update the surface
	SDL_UpdateWindowSurface( m_window );

	// SDL_SetRelativeMouseMode((SDL_bool) true);

	LOG(INFO, "SDL initialized");

	return true;
}

bool RenderManager::initGL()
{
	m_glContext = SDL_GL_CreateContext(m_window);

	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		LOG(ERROR, "GLew initialization error: "<<glewGetErrorString(err));
		return false;
	}
	LOG(INFO,"GLEW initialized");

	glClearColor(0, 0, 0, 1); // black

	glViewport(0,0,m_width,m_height);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);

	glEnable(GL_DEPTH_TEST);

	return true;
}

bool RenderManager::destroy()
{
	SDL_DestroyWindow( m_window );

	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	return true;
}

void RenderManager::preRender()
{
	//CLEAR
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);

	glEnable(GL_DEPTH_TEST);

	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT   | GL_ENABLE_BIT  |
	             GL_TEXTURE_BIT      | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);
}

void RenderManager::postRender()
{
	//SWAP BUFFERS
	SDL_GL_SwapWindow(m_window);

	glPopAttrib();
}

void RenderManager::onWindowResize(IEventDataPtr e)
{
	WindowResizedEventData* event = dynamic_cast<WindowResizedEventData*>(e.get());
	m_width = event->getWidth();
	m_height = event->getHeight();

	glViewport(0, 0, m_width, m_height);
}


// void RenderManager::onManagerPause(IEventDataPtr e)
// {
// 	SDL_SetRelativeMouseMode((SDL_bool) false);	
// }

// void RenderManager::onManagerResume(IEventDataPtr e)
// {
// 	SDL_SetRelativeMouseMode((SDL_bool) true);		
// }

