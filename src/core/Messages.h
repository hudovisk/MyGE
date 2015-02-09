
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

#endif