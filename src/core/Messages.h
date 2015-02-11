
#ifndef MESSAGES_H
#define MESSAGES_H


#include "math/Transform.h"

#include <memory>

typedef unsigned int EntityHandler;

class IMessageData;

typedef std::shared_ptr<IMessageData> IMessageDataPtr;

class IMessageData
{
public:
	IMessageData() : m_handled(false) { }
	virtual ~IMessageData() { }
	
	virtual const unsigned int getType() const = 0;

	void setEntityHandler(EntityHandler entity) { m_entity = entity; }
	EntityHandler getEntityHandler() { return m_entity; }

	bool isHandled() { return m_handled; }
	void setHandled(bool handled) { m_handled = handled; }

protected:
	bool m_handled;
	EntityHandler m_entity;
};

///////////////////////////////////////////////////////////////////////////////////////

class GetTransformMessage : public IMessageData
{
public:
	GetTransformMessage() : m_transform(nullptr) { }
	virtual ~GetTransformMessage() { }
	
	const unsigned int getType() const { return 0; }

	void setTransform(Transform* transform) { m_transform = transform; }
	Transform* getTransform() { return m_transform; }

private:
	Transform* m_transform;
};

///////////////////////////////////////////////////////////////////////////////////////

class PlayerInputMessage : public IMessageData
{
public:
	PlayerInputMessage() { }
	~PlayerInputMessage() { }

	const unsigned int getType() const { return 1; }

	int getId() const { return m_id; }
	void setId(int id) { m_id = id; }

	float getInputValue() const { return m_value; }
	float getInputX() const { return m_pos[0]; }
	float getInputY() const { return m_pos[1]; }

	void setInputValue(float value) { m_value = value; }
	void setInputX(float value) { m_pos[0] = value; }
	void setInputY(float value) { m_pos[1] = value; }
private:
	int m_id;
	union
	{
		float m_value;
		float m_pos[2];
	};
};

#endif