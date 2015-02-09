
#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "systems/MeshSystem.h"
#include "systems/SpatialSystem.h"
#include "systems/Component.h"
#include "core/Messages.h"

#include <map>

typedef unsigned int EntityHandler;

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	bool init();
	bool destroy();

	void loadEntity(const char* filePath);

	void sendMessage(IMessageDataPtr message);

private:
	MeshSystem m_meshSystem;
	SpatialSystem m_spatialSystem;

	bool m_isInitialised;

	std::map<EntityHandler, std::map<ComponentType, Component*>> m_entities;
	static unsigned int NEXT_ID;
};

#endif //ENTITY_MANAGER_H