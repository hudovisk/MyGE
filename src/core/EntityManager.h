
#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "systems/System.h"

#include "core/Messages.h"

#include <string>
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
	bool m_isInitialised;

	std::map<std::string, System*> m_systems;

	std::map<EntityHandler, std::map<std::string, Component*>> m_entities;
	static unsigned int NEXT_ID;
};

#endif //ENTITY_MANAGER_H