
#include "debug/DebugRenderManager.h"

#include "core/Engine.h"
#include "debug/Logger.h"

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <string>

DebugRenderManager::DebugRenderManager()
	:m_defaultFont(NULL), m_isRenderFps(false),
	 m_isInitialised(false)
{

}

DebugRenderManager::~DebugRenderManager()
{
	if(m_isInitialised)
	{
		LOG(WARN, "Forgot to explicitally call destroy before DebugRenderManager destructor, memmory leak?");
		destroy();
	}
}

// bool DebugRenderManager::loadDefaultFont(char* fontPath, int size)
// {
// 	m_defaultFont = TTF_OpenFont(fontPath, size);
// 	if(!m_defaultFont)
// 	{
// 		LOG(ERROR, "Couldn't load font: "<<fontPath<<" size: "<<size);
// 		return false;
// 	}

// 	TTF_SetFontStyle(m_defaultFont, TTF_STYLE_NORMAL);

// 	return true;
// }

bool DebugRenderManager::init()
{
	if(m_isInitialised)
	{
		LOG(WARN, "Calling DebugRenderManager.init() when already initialised.");
		return false;
	}
	if(TTF_Init() < 0)
	{
		LOG(ERROR, "SDL2_TTF could not initialise: "<<TTF_GetError());
		return false;
	}

	m_defaultFont = TTF_OpenFont("res/fonts/OpenSans-Regular.ttf", 40);
	if(!m_defaultFont)
	{
		LOG(ERROR, "Couldn't load font: "<<"res/fonts/lazy.ttf"<<" size: "<<40);
		return false;
	}

	TTF_SetFontStyle(m_defaultFont, TTF_STYLE_NORMAL);

	if(!m_defaultShader.init("res/shaders/debugVertex.glsl", 
		"res/shaders/debugFragment.glsl"))
	{
		LOG(ERROR, "Default DebugRender shader could not initialise.");
		return false;
	}

	initQuad();

	for(int i=0; i<16; i++)
		m_fpsTextureTransform[0] = 0;

	m_fpsTextureTransform[0] = m_fpsTextureTransform[5] = m_fpsTextureTransform[10] = m_fpsTextureTransform[15] = 1;
	m_fpsTextureTransform[3] = m_fpsTextureTransform[7] = -1.0;
	m_isInitialised = true;

	return true;
}

void DebugRenderManager::initQuad()
{
	float vertices[20] = 
	{ 	0 ,0, 0,
		0, 1,
		1, 0, 0,
		1, 1,
		1, 1, 0,
		1, 0, 
		0, 1, 0,
		0, 0
	};

	glGenVertexArrays(1, &m_vaoQuad);
	glBindVertexArray(m_vaoQuad);

	//initialise vertex buffer
	glGenBuffers(1,&m_vboQuad);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
		&vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); //position
	glEnableVertexAttribArray(1); //texture uv

	glVertexAttribPointer(
			0,                  	// attribute 0.
			3,										// size
			GL_FLOAT,           	// type
			GL_FALSE,           	// normalized?
			5*sizeof(float),   		// stride
			(void*)0            	// array buffer offset
	);
	glVertexAttribPointer(
			1,                  	// attribute 1.
			2,										// size
			GL_FLOAT,           	// type
			GL_FALSE,           	// normalized?
			5*sizeof(float),   		// stride
			(void*)12            	// array buffer offset
	);

	unsigned int indices[6] = {0, 1, 2, 2, 3, 0};

	glGenBuffers(1, &m_vboIndicesQuad);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIndicesQuad);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
							sizeof(indices),
							&indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

bool DebugRenderManager::destroy()
{
	if(!m_isInitialised)
	{
		LOG(WARN, "Calling DebugRenderManager.destroy() when not initialised.");
		return false;
	}

	m_fpsTexture.destroy();

	glDeleteBuffers(1, &m_vboQuad);
	glDeleteBuffers(1, &m_vboIndicesQuad);
	glDeleteVertexArrays(1, &m_vaoQuad);

	TTF_CloseFont(m_defaultFont);
	TTF_Quit();

	m_isInitialised = false;

	return true;
}

void DebugRenderManager::renderFps(bool isRenderFps)
{
	m_isRenderFps = isRenderFps;
}

void DebugRenderManager::setFps(float fps)
{
	std::string fpsString = "FPS: ";
	fpsString.append(std::to_string(fps));
	SDL_Color color = {255,255,255};

	m_fpsTexture.destroy();
	m_fpsTexture.initFromText(fpsString.c_str(), m_defaultFont, color);

	m_fpsTextureTransform[0] = m_fpsTexture.getWidth() / Engine::g_renderManager.getWidth();
	m_fpsTextureTransform[5] = m_fpsTexture.getHeight() / Engine::g_renderManager.getHeight();
}

void DebugRenderManager::render()
{
	if(!m_isRenderFps)
		return;
	glUseProgram(m_defaultShader.getProgram());

	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fpsTexture.getId());
	glBindVertexArray(m_vaoQuad);
	
	int uniformModelView = glGetUniformLocation(m_defaultShader.getProgram(),
							 "modelView_Matrix");
	if(-1 != uniformModelView)
	{
		glUniformMatrix4fv(uniformModelView, 1, false, m_fpsTextureTransform);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle
	}
	else
	{
		LOG(WARN,"modelView not found!");
	}
}