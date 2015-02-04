
#include "render/Texture.h"

#include "debug/Logger.h"

#include <GL/glew.h>

Texture::Texture()
	: m_id(0), m_width(0), m_height(0), m_isInitialised(false)
{

}

Texture::~Texture()
{
	destroy();
}

/**
 * @todo: Implement using SDL_Image
 */
bool Texture::initFromImg(const char* imgPath)
{
	return false;
}

bool Texture::initFromText(const char* text, TTF_Font* font, SDL_Color color)
{
	if(m_isInitialised)
	{
		LOG(WARN, "Calling initFromText with texture already initialised.");
		destroy();
	}

	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
    	GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glBindTexture(GL_TEXTURE_2D,0);

    m_width = surface->w;
    m_height = surface->h;

    SDL_FreeSurface(surface);

    m_isInitialised = true;

    return true;
}

bool Texture::destroy()
{
	if(!m_isInitialised)
		return false;

	glDeleteTextures(1, &m_id);
	m_isInitialised = false;
}