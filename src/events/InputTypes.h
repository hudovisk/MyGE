
#ifndef INPUT_TYPES_H
#define INPUT_TYPES_H

#include "SDL2/SDL_keycode.h"

enum class InputEventType
{
	UNKNOWN,
	KEYBOARD,
	MOUSE,
};

enum MouseCode
{
	MOUSE_MOTION,

	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_WHEEL_UP,
	MOUSE_WHEEL_DOWN,
};

class KeyboardEvent
{
public:
	SDL_Keycode m_code;
	bool m_pressed;
	
	bool operator<(const KeyboardEvent& rhs) const { 
		if(this->m_code == rhs.m_code) 
	    return this->m_pressed < rhs.m_pressed;
		else
			return this->m_code < rhs.m_code;
	}
};

class MouseEvent
{
public:
	MouseCode m_code;
	int m_posX;
	int m_posY;
	int m_relX;
	int m_relY;
	bool m_pressed;
};

class InputEvent
{
public:
	InputEvent(const float timeStamp)
		: m_type(InputEventType::UNKNOWN), m_timeStamp(timeStamp)
	{

	}

	InputEventType m_type;
	const float m_timeStamp;

	KeyboardEvent m_key;
	MouseEvent m_mouse;
};

#endif