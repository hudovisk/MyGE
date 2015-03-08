
#ifndef DEBUG_RENDER_MANAGER_H
#define DEBUG_RENDER_MANAGER_H

#include "render/Shader.h"
#include "render/Texture.h"
#include <SDL2/SDL_ttf.h>

/**
 * @brief Implements basic rendering functions for debugging porpuses.
 */
class DebugRenderManager
{
public:
	DebugRenderManager();
	~DebugRenderManager();
	
	bool init();
	bool destroy();

	// bool loadDefaultFont(char* fontPath, int size);

	void render();

	void renderFps(bool isRenderFps);
	bool isRenderFps() { return m_isRenderFps; }
	void setFps(float fps);
private:
	void initQuad();

	TTF_Font* m_defaultFont;
	Shader m_defaultShader;

	Texture* m_fpsTexture;
	Matrix4 m_fpsTextureMatrix;

	unsigned int m_vaoQuad;
	unsigned int m_vboQuad;
	unsigned int m_vboIndicesQuad;

	bool m_isRenderFps;

	bool m_isInitialised;
};

#endif //DEBUG_RENDER_MANAGER_H