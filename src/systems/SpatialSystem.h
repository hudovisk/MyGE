
#ifndef SPATIAL_SYSTEM_H
#define SPATIAL_SYSTEM_H

#include "systems/System.h"
#include "memory/ObjectPool.h"
#include "math/Transform.h"

class SpatialSystem;

class SpatialComponent : public Component
{
public:
	SpatialComponent() { }
	virtual ~SpatialComponent() { }

	void receiveMessage(IMessageDataPtr message);

private:
	friend class SpatialSystem;

	SpatialSystem* m_system;
	Transform m_transform;	
};

class SpatialSystem : public System
{
public:
	SpatialSystem();
	~SpatialSystem();

	bool init();
	bool destroy();

	void receiveMessage(SpatialComponent* component, IMessageDataPtr message);

	Component* create();
	Component* createFromJSON(const char* json);

	void release(Component* spatial);

private:
	bool m_isInitialised;	
	
	ObjectPool<SpatialComponent> m_componentPool;
	GetTransformMessage m_getTransformMsg;
};

#endif