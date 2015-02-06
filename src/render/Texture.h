
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
	unsigned int m_id;

	float m_width;
	float m_height;

	bool m_isInitialised;
};

#endif //TEXTURE_H