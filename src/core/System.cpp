
#include "core/System.h"
#include "debug/Logger.h"

#include <SDL2/SDL.h>


bool System::init()
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

bool System::destroy()
{
	SDL_QuitSubSystem(SDL_INIT_TIMER);

	SDL_Quit();

	return true;
}

int System::getHighResClock()
{
	return SDL_GetPerformanceCounter();
}

int System::getHighResClockFrequency()
{
	return SDL_GetPerformanceFrequency();
}

float System::getHighResTime()
{
	return SDL_GetPerformanceCounter() / (float) SDL_GetPerformanceFrequency();
}