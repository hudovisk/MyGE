
#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "systems/System.h"

#include "render/Geometric.h"
#include "render/GeometricPassShader.h"

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

	void geometricPass(IEventDataPtr e);
	void shadowStencilPass(IEventDataPtr e);

	Component* create();
	Component* createFromJSON(const rapidjson::Value& jsonObject);

	void release(Component* mesh);
private:
	// void loadFile(MeshComponent* mesh, std::string filePath);

	bool m_isInitialised;

	GeometricPassShader m_gpShader;
	Shader m_shadowStencilShader;
	
	GetTransformMessage m_getTransformMsg;

	ObjectPool<MeshComponent> m_componentPool;
};

#endif