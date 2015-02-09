
#ifndef SPATIAL_SYSTEM_H
#define SPATIAL_SYSTEM_H

#include "systems/Component.h"
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

class SpatialSystem
{
public:
	SpatialSystem();
	~SpatialSystem();

	bool init();
	bool destroy();

	void receiveMessage(SpatialComponent* component, IMessageDataPtr message);

	SpatialComponent* createSpatial();

	void releaseSpatial(SpatialComponent* spatial);

private:
	ObjectPool<SpatialComponent> m_spatialPool;
	std::shared_ptr<GetTransformMessage> m_getTransformMsg;

	bool m_isInitialised;	
};

#endif