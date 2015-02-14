
#include "render/RenderManager.h"
#include "debug/Logger.h"
#include "debug/MyAssert.h"
#include "core/Engine.h"

#include "core/Camera.h"

#include <iostream>

//UI32 RenderManager::NEXT_RENDER_COMPONENT_ID = 1;

bool RenderManager::init(int width, int height)
{
	m_width = width;
	m_height = height;

	if(!initSDL())
		return false;

	if(!initGL())
		return false;

	if(!m_texturesPool.init(10))
		return false;

	if(!m_geometricPool.init(100000))
		return false;

	if(!m_defaultShader.init("res/shaders/vertex.glsl", 
		"res/shaders/fragment.glsl"))
	{
		LOG(ERROR, "Default RenderManager shader could not initialise.");
		return false;
	}

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
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

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


void RenderManager::bindDefaultShader()
{
	glUseProgram(m_defaultShader.getProgram());
}

void RenderManager::setMatrixUniform(const char* uniformName, const Matrix4& value)
{
	int uniformId = glGetUniformLocation(m_defaultShader.getProgram(), uniformName);
	glUniformMatrix4fv(uniformId, 1, true, value.m_data);
}

void RenderManager::render(Geometric* geometric)
{
	if(!geometric->m_isInitialised)
		return;

	glBindVertexArray(geometric->m_vao);
	glDrawElements(GL_TRIANGLES, geometric->m_numIndices, GL_UNSIGNED_INT, 0);
}

void RenderManager::postRender()
{
	//SWAP BUFFERS
	SDL_GL_SwapWindow(m_window);

	glPopAttrib();
}

Texture* RenderManager::createTextureFromImg(const char* imgPath)
{
	ASSERT(true, "Unimplemented method call: RenderManager.createTextureFromImg");
	return nullptr;
}

void RenderManager::updateTextureFromText(Texture* texture,
 const char* text, TTF_Font* font, SDL_Color color)
{
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);

	glBindTexture(GL_TEXTURE_2D, texture->m_id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
    	GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glBindTexture(GL_TEXTURE_2D,0);

    texture->m_width = surface->w;
    texture->m_height = surface->h;

    SDL_FreeSurface(surface);

    texture->m_isInitialised = true;
}

Texture* RenderManager::createTextureFromText(const char* text, TTF_Font* font, SDL_Color color)
{
	Texture* texture = m_texturesPool.create();

	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);

	glGenTextures(1, &texture->m_id);
	glBindTexture(GL_TEXTURE_2D, texture->m_id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
    	GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glBindTexture(GL_TEXTURE_2D,0);

    texture->m_width = surface->w;
    texture->m_height = surface->h;

    SDL_FreeSurface(surface);

    texture->m_isInitialised = true;

    return texture;
}

void RenderManager::releaseTexture(Texture* texture)
{
	if(texture->m_isInitialised)
	{
		glDeleteTextures(1, &texture->m_id);
		texture->m_isInitialised = false;	
	}
	m_texturesPool.release(texture);
}

Geometric* RenderManager::createGeometric(const std::vector<Vertex> vertices,
	const std::vector<unsigned int>& indices)
{
	Geometric* geometric = m_geometricPool.create();

	glGenVertexArrays(1, &geometric->m_vao);
	glBindVertexArray(geometric->m_vao);

	//initialise vertex buffer
	glGenBuffers(1,&geometric->m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, geometric->m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 
							vertices.size() * sizeof(Vertex),
							&vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //normals
	glEnableVertexAttribArray(2); //texture uv

	glVertexAttribPointer(
			0,                  	// attribute 0.
			3,										// size
			GL_FLOAT,           	// type
			GL_FALSE,           	// normalized?
			sizeof(Vertex),    		// stride
			(void*)0            	// array buffer offset
	);
	glVertexAttribPointer(
			1,                  	// attribute 1.
			3,										// size
			GL_FLOAT,           	// type
			GL_FALSE,           	// normalized?
			sizeof(Vertex),    		// stride
			(void*)12            	// array buffer offset
	);
	glVertexAttribPointer(
			2,                  	// attribute 2.
			2,										// size
			GL_FLOAT,           	// type
			GL_FALSE,           	// normalized?
			sizeof(Vertex),    		// stride
			(void*)24            	// array buffer offset
	);

	glGenBuffers(1, &geometric->m_vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometric->m_vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
							indices.size() * sizeof(unsigned int),
							&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	geometric->m_numIndices = indices.size();
	geometric->m_isInitialised = true;

	return geometric;
}

void RenderManager::releaseGeometric(Geometric* geometric)
{
	if(geometric->m_isInitialised)
	{
		glDeleteBuffers(1, &geometric->m_vbo);
		glDeleteBuffers(1, &geometric->m_vboIndices);
		glDeleteVertexArrays(1, &geometric->m_vao);
		geometric->m_isInitialised = false;
	}

	m_geometricPool.release(geometric);
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

