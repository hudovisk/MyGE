
#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include "events/Events.h"
#include "render/Texture.h"
#include "render/Geometric.h"
#include "render/Shader.h"

#include "memory/ObjectPool.h"

#include "math/Matrix4.h"

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
	void render(Geometric* geometric);
	void postRender();

	void bindDefaultShader();
	// void BindShader(Shader shader);

	void setMatrixUniform(const char* uniformName, const Matrix4& value);

	/**
	 * @brief Initiale texture from image.
	 * @details Unimplemented
	 * 
	 * @param imgPath The file path of the image.
	 * @return A texture pointer, must be released by RenderManager.
	 * 
	 * @todo implement using SDL_Image
	 */
	Texture* createTextureFromImg(const char* imgPath);

	/**
	 * @brief Initialise texture from SDL_TTF font.
	 * @details Utilise the TTF_RenderUTF8_Blended, (expensive).
	 * 
	 * @param text The text to be printed
	 * @param font The TTF_Font.
	 * @param color The color of the text, rgb.
	 * 
	 * @return A texture pointer, must be released by RenderManager.
	 */
	Texture* createTextureFromText(const char* text, TTF_Font* font, SDL_Color color);

	/**
	 * @brief Update texture from SDL_TTF font.
	 * @details Utilise the TTF_RenderUTF8_Blended, (expensive).
	 * 
	 * @param texture The texture to be updated
	 * @param text The text to be printed
	 * @param font The TTF_Font.
	 * @param color The color of the text, rgb.
	 */
	void updateTextureFromText(Texture* texture, 
		const char* text, TTF_Font* font, SDL_Color color);

	/**
	 * @brief Releases the texture pointer.
	 * @details  Just wrappes ObjectPool release and deletes the texture cache from OpenGL.
	 * 
	 * @param texture The texture pointer to be released.
	 */
	void releaseTexture(Texture* texture);

	/**
	 * @brief Initialise a Geometric object.
	 * @details Utilises the assimp library to open the file and extract all geometric information.
	 * 
	 * @param filePath the Path of the file.
	 * @return Geometric object initialised.
	 */
	Geometric* createGeometric(const std::vector<Vertex> vertices,
	const std::vector<unsigned int>& indices);

	/**
	 * @brief Releases the geometric pointer.
	 * @details  Just wrappes ObjectPool release and deletes the geometric cache from OpenGL.
	 * 
	 * @param texture The geometric pointer to be released.
	 */
	void releaseGeometric(Geometric* geometric);


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

	Shader m_defaultShader;

	ObjectPool<Texture> m_texturesPool;
	ObjectPool<Geometric> m_geometricPool;

	//static unsigned int NEXT_RENDER_COMPONENT_ID;
	//std::vector<RenderComponentPtr> m_renderComponents;
};

#endif