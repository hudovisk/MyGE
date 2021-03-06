
#include "core/Engine.h"

#include "render/RenderManager.h"

#include "core/ResourceManager.h"

#include "debug/Logger.h"
#include "debug/MyAssert.h"

#include <iostream>

bool RenderManager::init(int width, int height)
{
	m_width = width;
	m_height = height;

	if(!initSDL())
		return false;

	if(!initGL())
		return false;

	if(!m_texturesPool.init(100))
		return false;

	if(!m_geometricPool.init(100000))
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
	if(!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF))
	{
		LOG(ERROR, "SDL_Image init error: " << SDL_GetError());
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

	SDL_SetRelativeMouseMode((SDL_bool) true);

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

	if(!initGBuffer())
		return false;

	if(!initShadowMapBuffer())
		return false;

	return true;
}

bool RenderManager::initShadowMapBuffer()
{
	// Create the FBO (Draws off screen, into the FBO)
    glGenFramebuffers(1, &m_smFbo); 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_smFbo);

    // attach DepthTexture 
    glGenTextures(1, &m_smDepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_smDepthTexture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_width, m_height, 0, 
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		m_smDepthTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        LOG(ERROR, "Framebuffer Object creation error, status: "<<Status);
        return false;
    }

    //Restore default FBO.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

bool RenderManager::initGBuffer()
{
	// Create the FBO (Draws off screen, into the FBO)
    glGenFramebuffers(1, &m_gbFbo); 
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_gbFbo);

	// Create the gbuffer textures (Create and attach texts to FBO)
  	glGenTextures(GBUFFER_NUM_TEXTURES, m_gbTextures);
  	for (unsigned int i = 0 ; i < GBUFFER_NUM_TEXTURES; i++) {
		glBindTexture(GL_TEXTURE_2D, m_gbTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 
			0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
			GL_TEXTURE_2D, m_gbTextures[i], 0);
	}

	// Same as before but for depth. DepthTexture is attached differently
	glGenTextures(1, &m_gbDepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_gbDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_width, m_height, 0, 
		GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		m_gbDepthTexture, 0);

	//final texture
	glGenTextures(1, &m_gbFinalTexture);
	glBindTexture(GL_TEXTURE_2D, m_gbFinalTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_width, m_height, 0, 
		GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_FINAL, GL_TEXTURE_2D,
		m_gbFinalTexture, 0);


	//Enable the attached textures to be written.
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_POSITION,
							 GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_NORMAL, 
							 GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_DIFFUSE }; 

    glDrawBuffers(3, drawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        LOG(ERROR, "Framebuffer Object creation error, status: "<<Status);
        return false;
    }

    //Restore default FBO.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	return true;
}

bool RenderManager::destroy()
{
	//GBuffer cleanup
	glDeleteTextures(GBUFFER_NUM_TEXTURES, m_gbTextures);
	glDeleteTextures(1, &m_gbDepthTexture);
	glDeleteFramebuffers(1, &m_gbFbo);
	
	SDL_DestroyWindow( m_window );

	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	return true;
}

void RenderManager::bindGeometricPass()
{
	//Screen to gbuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_gbFbo);

	//Enable the attached textures to be written.
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_POSITION,	
							 GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_NORMAL,
							 GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_DIFFUSE,
							 GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_FINAL }; 

    glDrawBuffers(4, drawBuffers);
    glReadBuffer(GL_NONE);

    // Only the geometry pass updates the depth buffer
    glDepthMask(GL_TRUE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);



	// glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT   | GL_ENABLE_BIT  |
	//              GL_TEXTURE_BIT      | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);
}

void RenderManager::bindShadowPass()
{
	glDepthMask(GL_TRUE);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_smFbo);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);
}

void RenderManager::bindLightPass()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_gbFbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_FINAL);
	// glClear(GL_COLOR_BUFFER_BIT); //CLEAR FINAL BUFFER

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
   	glBlendEquation(GL_FUNC_ADD);
   	glBlendFunc(GL_ONE, GL_ONE);	

	//Read from GBuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbFbo);

	for(unsigned int i=0; i < GBUFFER_NUM_TEXTURES; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_gbTextures[i]);
	}

	//Read from ShadowMapBuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_smFbo);
	glActiveTexture(GL_TEXTURE0 + SHADOWMAP_TEXTURE_DEPTH);
	glBindTexture(GL_TEXTURE_2D, m_smDepthTexture);
	// glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	// glTexParameteri (GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
}

void RenderManager::bindSkyboxPass()
{
	glDisable(GL_BLEND);
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
}

void RenderManager::renderGBuffer()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_gbFbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_FINAL);
	//CLEAR
	glClear(GL_COLOR_BUFFER_BIT);

	//Read from GBuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbFbo);

	GLsizei HalfWidth = (GLsizei)(m_width / 2.0f);
    GLsizei HalfHeight = (GLsizei)(m_height / 2.0f);

	glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_POSITION);
	glBlitFramebuffer(0, 0, m_width, m_height, 
		0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_DIFFUSE);
    glBlitFramebuffer(0, 0, m_width, m_height, 
    	0, HalfHeight, HalfWidth, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_NORMAL);
    glBlitFramebuffer(0, 0, m_width, m_height, 
        HalfWidth, HalfHeight, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_FINAL);
    // glBlitFramebuffer(0, 0, m_width, m_height, 
    //     HalfWidth, 0, m_width, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);	
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
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbFbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + GBUFFER_TEXTURE_TYPE_FINAL);
    glBlitFramebuffer(0, 0, m_width, m_height, 
                      0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	//SWAP BUFFERS
	SDL_GL_SwapWindow(m_window);

	// glPopAttrib();
}

void RenderManager::bindShader(const Shader& shader)
{
	glUseProgram(shader.m_program);
}

void RenderManager::bind2DTexture(unsigned int offset, const Texture* texture)
{
	if(!texture->m_isInitialised)
		return;

	glActiveTexture(GL_TEXTURE0 + offset);
	glBindTexture(GL_TEXTURE_2D, texture->m_id);
}

void RenderManager::bindCubeMapTexture(unsigned int offset, unsigned int id)
{
	glActiveTexture(GL_TEXTURE0 + offset);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void RenderManager::buildDefaultProjectionMatrix()
{
	float ymax = m_defaultCamera->m_nearDistance * tanf(m_defaultCamera->m_fov/2);
	float xmax = ymax*m_defaultCamera->m_aspectRatio;

	float temp = 2 * m_defaultCamera->m_nearDistance;
	float temp2 = 2 * xmax;
	float temp3 = 2 * ymax;
	float temp4 = m_defaultCamera->m_farDistance - m_defaultCamera->m_nearDistance;

	m_defaultCamera->m_projectionMatrix.m_data[0] = temp / temp2;
	m_defaultCamera->m_projectionMatrix.m_data[1] = 0;
	m_defaultCamera->m_projectionMatrix.m_data[2] = 0;
	m_defaultCamera->m_projectionMatrix.m_data[3] = 0;

	m_defaultCamera->m_projectionMatrix.m_data[4] = 0;
	m_defaultCamera->m_projectionMatrix.m_data[5] = temp / temp3;
	m_defaultCamera->m_projectionMatrix.m_data[6] = 0;
	m_defaultCamera->m_projectionMatrix.m_data[7] = 0;

	m_defaultCamera->m_projectionMatrix.m_data[8] = 0;
	m_defaultCamera->m_projectionMatrix.m_data[9] = 0;
	m_defaultCamera->m_projectionMatrix.m_data[10] = - (m_defaultCamera->m_farDistance + m_defaultCamera->m_nearDistance) / temp4;
	m_defaultCamera->m_projectionMatrix.m_data[11] = -1;//(-temp * m_farDistance) / temp4;

	m_defaultCamera->m_projectionMatrix.m_data[12] = 0;
	m_defaultCamera->m_projectionMatrix.m_data[13] = 0;
	m_defaultCamera->m_projectionMatrix.m_data[14] = (-temp * m_defaultCamera->m_farDistance) / temp4;//-1;
	m_defaultCamera->m_projectionMatrix.m_data[15] = 0;

	m_defaultCamera->m_dirty = false;
}

void RenderManager::setDefaultCamera(CameraComponent* camera)
{
	GetTransformMessage message;
	message.setHandled(false);
	message.setEntityHandler(camera->m_entity);
	
	Engine::g_entityManager.sendMessage(&message);
	
	if(message.isHandled())
	{
		m_defaultCamera = camera;
		m_defaultCameraTransform = message.getTransform();
	}
	else
	{
		LOG(WARN, "Default camera: "<<camera->m_entity<<" should have a position component");
	}

}

const Matrix4& RenderManager::getDefaultCameraProjection()
{
	if(m_defaultCamera->m_dirty)
	{
		buildDefaultProjectionMatrix();
	}
	return m_defaultCamera->m_projectionMatrix;
}

Transform* RenderManager::getDefaultCameraTransform()
{
	return m_defaultCameraTransform;
}

unsigned int RenderManager::createCubeMapTexFromImgs(std::vector<std::string> files)
{
	if(files.size() != 6)
	{
		LOG(ERROR, "Cube maps must have 6 image files.");
		return 0;
	}

	unsigned int id = 0;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);

	for(int i=0; i < 6; i++)
	{
		SDL_Surface* surface = IMG_Load(files[i].c_str());

		int mode = GL_RGB; 
		if(surface->format->BytesPerPixel == 4) {
		    mode = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mode, surface->w, surface->h, 0,
    		mode, GL_UNSIGNED_BYTE, surface->pixels);

		SDL_FreeSurface(surface);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return id;
}

Texture* RenderManager::createTextureFromImg(std::string imgPath)
{
	Texture* texture = m_texturesPool.create();
	SDL_Surface* surface = IMG_Load(imgPath.c_str());

	int mode = GL_RGB;
 
	if(surface->format->BytesPerPixel == 4) {
	    mode = GL_RGBA;
	}

	glGenTextures(1, &texture->m_id);
	glBindTexture(GL_TEXTURE_2D, texture->m_id);
	glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0,
    	mode, GL_UNSIGNED_BYTE, surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D); // allocate mipmaps
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D,0);

    texture->m_width = surface->w;
    texture->m_height = surface->h;

    SDL_FreeSurface(surface);

    texture->m_isInitialised = true;

    // std::cout<<"Id: "<<texture->m_id<<" Address: "<<texture<<std::endl;

    return texture;
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

void RenderManager::releaseCubeMap(unsigned int id)
{
	glDeleteTextures(1, &id);
}

Geometric* RenderManager::createGeometric(float *verticesData, unsigned int numVertices,
		unsigned int* indices, unsigned int numIndices, int flags)
{
	Geometric* geometric = m_geometricPool.create();

	unsigned int dataSize = 3;
	if(flags & ResourceManager::LOAD_NORMALS) dataSize += 3;
	if(flags & ResourceManager::LOAD_TEXTURES) dataSize += 2;
	if(flags & ResourceManager::LOAD_TANGENTS) dataSize += 3;

	glGenVertexArrays(1, &geometric->m_vao);
	glBindVertexArray(geometric->m_vao);

	//initialise vertex buffer
	glGenBuffers(1,&geometric->m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, geometric->m_vbo);
	glBufferData(GL_ARRAY_BUFFER, numVertices * dataSize * sizeof(float), verticesData, GL_STATIC_DRAW);

	unsigned int attrCounter = 0;
	unsigned int offset = 0;

	glEnableVertexAttribArray(attrCounter); //position
	glVertexAttribPointer(
			attrCounter,            	// attribute 0.
			3,							// size
			GL_FLOAT,           		// type
			GL_FALSE,           		// normalized?
			dataSize * sizeof(float),   // stride
			(void*)offset          		// array buffer offset
	);
	attrCounter++;
	offset += 3 * sizeof(float); 

	if(flags & ResourceManager::LOAD_NORMALS)
	{
		glEnableVertexAttribArray(attrCounter); //normals
		glVertexAttribPointer(
			attrCounter,                // attribute 1.
			3,							// size
			GL_FLOAT,           		// type
			GL_FALSE,           		// normalized?
			dataSize * sizeof(float),   // stride
			(void*)offset          		// array buffer offset
		);
		attrCounter++;
		offset += 3 * sizeof(float);
	}
	if(flags & ResourceManager::LOAD_TEXTURES)
	{
		glEnableVertexAttribArray(attrCounter); //texture uv
		glVertexAttribPointer(
			attrCounter,                  	// attribute 2.
			2,								// size
			GL_FLOAT,           			// type
			GL_FALSE,           			// normalized?
			dataSize * sizeof(float),    	// stride
			(void*)offset          			// array buffer offset
		);
		attrCounter++;
		offset += 2 * sizeof(float);
	}
	if(flags & ResourceManager::LOAD_TANGENTS)
	{
		glEnableVertexAttribArray(attrCounter); //tangents
		glVertexAttribPointer(
			attrCounter,                  	// attribute 2.
			3,										// size
			GL_FLOAT,           	// type
			GL_FALSE,           	// normalized?
			dataSize * sizeof(float),    		// stride
			(void*)offset            	// array buffer offset
		);
		attrCounter++;
		offset += 2 * sizeof(float);
	}	

	glGenBuffers(1, &geometric->m_vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometric->m_vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	geometric->m_numIndices = numIndices;
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

void RenderManager::onEnginePaused()
{
	SDL_SetRelativeMouseMode((SDL_bool) false);	
}

void RenderManager::onEngineResumed()
{
	SDL_SetRelativeMouseMode((SDL_bool) true);		
}

// void RenderManager::onManagerPause(IEventDataPtr e)
// {
// 	
// }

// void RenderManager::onManagerResume(IEventDataPtr e)
// {
// 	
// }

