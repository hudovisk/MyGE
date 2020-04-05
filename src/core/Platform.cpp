
#include "Platform.h"
#include "debug/Logger.h"

#include <SDL2/SDL.h>


bool Platform::init()
{
	if(SDL_Init(0))
	{
		LOG(ERROR, "SDL init error: " << SDL_GetError());
		return false;
	}

	if(SDL_InitSubSystem(SDL_INIT_TIMER))
	{
		LOG(ERROR, "SDL init timer error: " << SDL_GetError());
		return false;
	}

	return true;
}

bool Platform::destroy()
{
	SDL_QuitSubSystem(SDL_INIT_TIMER);

	SDL_Quit();

	return true;
}

int Platform::getHighResClock()
{
	return SDL_GetPerformanceCounter();
}

int Platform::getHighResClockFrequency()
{
	return SDL_GetPerformanceFrequency();
}

float Platform::getHighResTime()
{
	return SDL_GetPerformanceCounter() / (float) SDL_GetPerformanceFrequency();
}