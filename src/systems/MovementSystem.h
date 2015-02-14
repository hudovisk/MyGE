
#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "systems/System.h"
#include "events/Events.h"
#include "memory/ObjectPool.h"

class MovementSystem;

class MovementComponent : public Component
{
public:
	MovementComponent() { }
	~MovementComponent() { }

	void receiveMessage(IMessageDataPtr message);

private:
	friend class MovementSystem;

	MovementSystem* m_system;
	float m_maxVelocity;
	float m_currentVelocity;
	Vec3 m_localDirection;
};

class MovementSystem : public System
{
public:
	MovementSystem();
	~MovementSystem();

	bool init();
	bool destroy();

	void onUpdate(IEventDataPtr e);

	void receiveMessage(MovementComponent* component, IMessageDataPtr message);

	Component* createFromJSON(const char* json);
	Component* create();

	void release(Component* movementComponent);

private:
	bool m_isInitialised;

	unsigned int m_movementMessageType;
	GetTransformMessage m_getTransformMsg;

	ObjectPool<MovementComponent> m_componentPool;
};

#endif //MOVEMENT_SYSTEM_H