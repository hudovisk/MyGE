#ifndef EVENTS_H
#define EVENTS_H

#include "math/Matrix4.h"

#include <memory>

class IEventData;

typedef std::shared_ptr<IEventData> IEventDataPtr;

class IEventData
{
public:
	virtual ~IEventData() { }
	virtual const unsigned int getType() const = 0;
	virtual float getTimeStamp() const = 0;
//	virtual IEventData& copy() const = 0;
};

/**
 * @brief Base for all event data types
 * @details For each new event type, it must derive from this class and implement the getType method. Which must return 
 * a diferent number of any other type
 * 
 * @todo An easier way to create new event types, without looking for all the other events before choosing a value for type,
 * 	Should look at Modern C++ Design by Andrei Alexandrescu for a solution.
 */
class BaseEventData : public IEventData
{
const float m_timeStamp;

public:
	BaseEventData(const float timeStamp = 0.0f) : m_timeStamp(timeStamp) { }

	// Returns the type of the event
	virtual const unsigned int getType() const = 0;

	float getTimeStamp(void) const { return m_timeStamp; }
};

class WindowClosedEventData : public BaseEventData
{
public:
	WindowClosedEventData(const float timeStamp = 0.0f) :
		BaseEventData(timeStamp)
	{

	}

	const unsigned int getType() const { return 0; }

//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

class WindowResizedEventData : public BaseEventData
{
	unsigned int m_width;
	unsigned int m_height;
public:
	WindowResizedEventData(const float timeStamp = 0.0f, int width = 0, int height = 0) :
		BaseEventData(timeStamp), m_width(width), m_height(height)
	{

	}

	const unsigned int getType() const { return 1; }

	const unsigned int getWidth() const { return m_width; }
	const unsigned int getHeight() const { return m_height; }
//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

class EnginePausedEventData : public BaseEventData
{
public:
	EnginePausedEventData(const float timeStamp = 0.0f) :
		BaseEventData(timeStamp)
	{

	}

	//must be a diferent number!!
	const unsigned int getType() const { return 2; }

//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

class EngineResumedEventData : public BaseEventData
{
public:
	EngineResumedEventData(const float timeStamp = 0.0f) :
		BaseEventData(timeStamp)
	{

	}

	//must be a diferent number!!
	const unsigned int getType() const { return 3; }

//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

class UpdateStageEventData : public BaseEventData
{
	float m_dt;
public:
	UpdateStageEventData(const float timeStamp = 0.0f) :
		BaseEventData(timeStamp)
	{

	}

	//must be a diferent number!!
	const unsigned int getType() const { return 4; }

	void setDeltaT(float dt) { m_dt = dt; }
	float getDeltaT() { return m_dt; }
//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

class PreRenderStageEventData : public BaseEventData
{
public:
	PreRenderStageEventData(const float timeStamp = 0.0f) :
		BaseEventData(timeStamp)
	{

	}

	//must be a diferent number!!
	const unsigned int getType() const { return 5; }
	
//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

class GeometricPassEventData : public BaseEventData
{
public:
	GeometricPassEventData(const float timeStamp = 0.0f) :
		BaseEventData(timeStamp)
	{

	}

	//must be a diferent number!!
	const unsigned int getType() const { return 6; }
	
//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

class ShadowStencilPassEventData : public BaseEventData
{
public:
	ShadowStencilPassEventData(const float timeStamp = 0.0f) :
		BaseEventData(timeStamp), m_stencil(false), m_projection(nullptr), m_view(nullptr)
	{

	}

	//must be a diferent number!!
	const unsigned int getType() const { return 7; }

	bool m_stencil;
	const Matrix4* m_projection;
	const Matrix4* m_view;
	
//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

class LightPassEventData : public BaseEventData
{
public:
	LightPassEventData(const float timeStamp = 0.0f) :
		BaseEventData(timeStamp)
	{

	}

	//must be a diferent number!!
	const unsigned int getType() const { return 8; }
	
//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

class PostRenderStageEventData : public BaseEventData
{
public:
	PostRenderStageEventData(const float timeStamp = 0.0f) :
		BaseEventData(timeStamp)
	{

	}

	//must be a diferent number!!
	const unsigned int getType() const { return 9; }
	
//	IEventData& copy() const { return WindowClosedEventData(m_timeStamp);}
};

#endif