
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
	/**
	 * @brief Default constructor
	 */
	Texture();

	/**
	 * @brief Default destructor
	 */
	~Texture();

	/**
	 * @brief Initiale texture from image.
	 * @details Unimplemented
	 * 
	 * @param imgPath The file path of the image.
	 * @return true if success, false otherwise.
	 */
	bool initFromImg(const char* imgPath);

	/**
	 * @brief Initialise texture from SDL_TTF font.
	 * @details Utilise the TTF_RenderUTF8_Blended, (expensive).
	 * 
	 * @param text The text to be printed
	 * @param font The TTF_Font.
	 * @param color The color of the text, rgb.
	 * 
	 * @return true if success, false otherwise.
	 */
	bool initFromText(const char* text, TTF_Font* font, SDL_Color color);

	/**
	 * @brief Cleanup method
	 * @details Destroy OpenGL Texture
	 * @return true if success, false otherwise.
	 */
	bool destroy();

	float getWidth() { return m_width; }
	float getHeight() { return m_height; }	

	unsigned int getId() { return m_id; }

private:
	unsigned int m_id;

	float m_width;
	float m_height;

	bool m_isInitialised;
};

#endif //TEXTURE_H