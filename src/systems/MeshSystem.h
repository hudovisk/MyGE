
#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "systems/Component.h"
#include "render/Geometric.h"
#include "memory/ObjectPool.h"
#include "events/Events.h"

#include <vector>

class EntityManager;
class MeshSystem;

class MeshComponent : public Component
{
public:
	MeshComponent() { }
	virtual ~MeshComponent() { }

	// void receiveMessage(IMessageDataPtr message)
	// {
	// 	system->receiveMessage(message);
	// }

private:
	friend class MeshSystem;
	
	std::vector<Geometric*> m_geometrics;	
	// MeshSystem* m_system;
};

class MeshSystem
{
public:
	MeshSystem();
	~MeshSystem();

	bool init();
	bool destroy();

	void onUpdate(IEventDataPtr e);

	// void receiveMessage(IMessageDataPtr message);

	MeshComponent* createMesh();
	MeshComponent* createMeshFromFile(const char* filePath);

	void releaseMesh(MeshComponent* mesh);
private:
	ObjectPool<MeshComponent> m_meshPool;
	std::shared_ptr<GetTransformMessage> m_getTransformMsg;

	bool m_isInitialised;
};

#endif