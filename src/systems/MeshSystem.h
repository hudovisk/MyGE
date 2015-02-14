
#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "systems/System.h"
#include "render/Geometric.h"
#include "memory/ObjectPool.h"
#include "events/Events.h"

#include <vector>

class MeshSystem;

class MeshComponent : public Component
{
public:
	MeshComponent() { }
	virtual ~MeshComponent() { }

private:
	friend class MeshSystem;
	
	std::vector<Geometric*> m_geometrics;	
};

class MeshSystem : public System
{
public:
	MeshSystem();
	~MeshSystem();

	bool init();
	bool destroy();

	void onUpdate(IEventDataPtr e);

	Component* create();
	Component* createFromJSON(const char* json);

	void release(Component* mesh);
private:
	bool m_isInitialised;

	GetTransformMessage m_getTransformMsg;

	ObjectPool<MeshComponent> m_componentPool;
};

#endif