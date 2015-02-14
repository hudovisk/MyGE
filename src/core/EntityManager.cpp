
#include "core/EntityManager.h"

#include "systems/MeshSystem.h"
#include "systems/SpatialSystem.h"
#include "systems/InputSystem.h"
#include "systems/MovementSystem.h"
#include "systems/PlayerControllerSystem.h"

#include "core/Engine.h"

#include "rapidjson/document.h"

#include <iostream>

unsigned int EntityManager::NEXT_ID = 1;

EntityManager::EntityManager()
	:m_isInitialised(false)
{

}

EntityManager::~EntityManager()
{
	destroy();
}

bool EntityManager::init()
{
	if(m_isInitialised)
		return false;

	m_systems["MeshSystem"] = new MeshSystem();
	m_systems["SpatialSystem"] = new SpatialSystem();
	m_systems["InputSystem"] = new InputSystem();
	m_systems["MovementSystem"] = new MovementSystem();
	m_systems["PlayerControllerSystem"] = new PlayerControllerSystem();

	for(auto itSystem = m_systems.begin(); itSystem != m_systems.end(); itSystem++)
	{
		if(!itSystem->second->init())
		{
			LOG(ERROR, "System: "<<itSystem->first<<" failed to initialise.");
			return false;
		}
	}

	m_isInitialised = true;

	return true;
}

bool EntityManager::destroy()
{
	if(m_isInitialised)
	{
		//RELEASE COMPONENTS
		for(auto itEntity = m_entities.begin(); itEntity != m_entities.end(); itEntity++)
		{
			for(auto itComponent = itEntity->second.begin(); 
				itComponent != itEntity->second.end(); itComponent++)
			{
				m_systems[itComponent->first]->release(itComponent->second);
			}
		}

		//DESTROY SYSTEMS
		for(auto itSystem = m_systems.begin(); itSystem != m_systems.end(); itSystem++)
		{
			if(!itSystem->second->destroy())
			{
				LOG(ERROR, "System: "<<itSystem->first<<" failed to destroy.");
				return false;
			}
		}

		m_isInitialised = false;
	}	
	return true;
}

void EntityManager::loadEntities(std::string filePath)
{
	FILE* file = fopen(filePath.c_str(), "rb");
	// ASSERT(file != nullptr, "Couldn't open file: "<<filePath);
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);

	fseek(file, 0L, SEEK_SET);

	char* buffer = (char*) calloc(size,sizeof(char));
	if(!buffer)
	{
		// LOG(ERROR, "File: "<<json<<" too big");
		fclose(file);

		// ASSERT(false,"File: "<<filePath<<" too big");
	}
	fread(buffer, 1, size, file);

	rapidjson::Document doc;
	doc.Parse<0>(buffer);

	for(auto itDoc = doc.Begin(); itDoc != doc.End(); itDoc++)
	{
		EntityHandler entity = EntityManager::NEXT_ID++;
		for(auto itMember = itDoc->MemberBegin();
			itMember != itDoc->MemberEnd(); itMember++)
		{
			if(strcmp("name", itMember->name.GetString()) == 0)
			{
				LOG(INFO, "Loading entity id: "<<entity<<"Name: "<<itMember->value.GetString());
			}
			else
			{
				auto itSystem = m_systems.find(itMember->name.GetString());
				if(itSystem != m_systems.end())
				{
					Component* component = itSystem->second->createFromJSON(itMember->value);
					component->m_entity = entity;
					m_entities[entity].insert(std::pair<std::string, Component*>(itMember->name.GetString(), component));				
				}
			}
		}
	}

	free(buffer);
	fclose(file);
}

void EntityManager::loadEntity(const char* filePath)
{
	// EntityHandler entity = EntityManager::NEXT_ID++;

	// Component* mesh = m_systems["MeshSystem"]->createFromJSON(filePath);
	// mesh->m_entity = entity;
	// m_entities[entity].insert(std::pair<std::string, Component*>("MeshSystem", mesh));

	// Component* spatial = m_systems["SpatialSystem"]->create();
	// spatial->m_entity = entity;	
	// m_entities[entity].insert(std::pair<std::string, Component*>("SpatialSystem", spatial));

	// Component* movement = m_systems["MovementSystem"]->create();
	// movement->m_entity = entity;
	// m_entities[entity].insert(std::pair<std::string, Component*>("MovementSystem", movement));
}

void EntityManager::loadPlayer(const char* filePath)
{
	// EntityHandler entity = EntityManager::NEXT_ID++;

	// Component* mesh = m_systems["MeshSystem"]->createFromJSON(filePath);
	// mesh->m_entity = entity;
	// m_entities[entity].insert(std::pair<std::string, Component*>("MeshSystem", mesh));

	// Component* spatial = m_systems["SpatialSystem"]->create();
	// spatial->m_entity = entity;	
	// m_entities[entity].insert(std::pair<std::string, Component*>("SpatialSystem", spatial));

	// Component* input = m_systems["InputSystem"]->createFromJSON("res/scripts/playerInputMap.xml");
	// input->m_entity = entity;	
	// m_entities[entity].insert(std::pair<std::string, Component*>("InputSystem", input));

	// Component* movement = m_systems["MovementSystem"]->create();
	// movement->m_entity = entity;
	// m_entities[entity].insert(std::pair<std::string, Component*>("MovementSystem", movement));

	// Component* player = m_systems["PlayerControllerSystem"]->create();
	// player->m_entity = entity;
	// m_entities[entity].insert(std::pair<std::string, Component*>("PlayerControllerSystem", player));	
}

void EntityManager::sendMessage(IMessageDataPtr message)
{
	auto itEntity = m_entities.find(message->getEntityHandler());
	if(itEntity != m_entities.end())
	{
		for(auto itComponent = itEntity->second.begin(); itComponent != itEntity->second.end();
		 itComponent++)
		{
			itComponent->second->receiveMessage(message);
		}
	}
	else
	{
		LOG(ERROR, "Entity id: "<<message->getEntityHandler()<<
			" not found in EntityManager::sendMessage."<<" Message type: "<<message->getType());
	}
}