
#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * @brief Platform especific methods.
 * @details For now, this class is reponsible for intialise SDL and use it for aquire time.
 * Its a layer of abstraction for SDL.
 */
class Platform
{
public:
	/**
	 * @brief Initialise SDL and the timer module.
	 * @return true on success, false otherwise.
	 */
	static bool init();

	/**
	 * @brief Cleanup method.
	 * @details cleanup only the timer module of SDL.
	 * @return true on success, false otherwise.
	 */
	static bool destroy();

	/**
	 * @return SDL_GetPerformanceCounter()
	 */
	static int getHighResClock();

	/**
	 * @return SDL_GetPerformanceFrequency()
	 */
	static int getHighResClockFrequency();

	/**
	 * @brief Gets current time.
	 * @details divide HighResClock by HighResrClockFrequency
	 * @return time in seconds
	 */
	static float getHighResTime();

};

#endif //PLATFORM_H