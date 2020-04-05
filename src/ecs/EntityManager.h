
#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "systems/System.h"
#include "Messages.h"

#include "rapidjson/document.h"

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

	void sendMessage(IMessageDataPtr message);

private:
	/**
	 * @brief Get a buffer filled with the contests of the file.
	 * @details [long description]
	 * 
	 * @param filePath path of the file
	 * @return the pointer to the buffer, must be freed with free(buffer)
	 */
	char* getFileBuffer(std::string filePath);

	void loadEntities(std::string filePath);

	bool m_isInitialised;

	std::map<std::string, System*> m_systems;

	std::map<EntityHandler, std::map<std::string, Component*>> m_entities;
	std::map<std::string, EntityHandler> m_entitiesNames;

	static unsigned int NEXT_ID;
};

#endif //ENTITY_MANAGER_H