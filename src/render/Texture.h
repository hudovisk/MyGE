
#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/**
 * @brief Texture class
 */
class Texture
{
public:
	Texture() 
		: m_id(0), m_width(0), m_height(0),
		m_isInitialised(false) { }

	unsigned int m_id;

	float m_width;
	float m_height;

	bool m_isInitialised;
};

#endif //TEXTURE_H