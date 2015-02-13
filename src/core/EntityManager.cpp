
#include "core/EntityManager.h"

#include "systems/MeshSystem.h"
#include "systems/SpatialSystem.h"
#include "systems/PlayerInputSystem.h"
#include "systems/MovementSystem.h"

#include "core/Engine.h"

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
	m_systems["PlayerInputSystem"] = new PlayerInputSystem();
	m_systems["MovementSystem"] = new MovementSystem();

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
				LOG(INFO, "System: "<<itComponent->first);
				m_systems[itComponent->first]->release(itComponent->second);
			}
		}

		//DESTROY SYSTEMS
		for(auto itSystem = m_systems.begin(); itSystem != m_systems.end(); itSystem++)
		{
			LOG(ERROR, "System: "<<itSystem->first<<" to destroy.");
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

void EntityManager::loadEntity(const char* filePath)
{
	EntityHandler entity = EntityManager::NEXT_ID++;

	Component* mesh = m_systems["MeshSystem"]->createFromJSON(filePath);
	mesh->m_entity = entity;
	m_entities[entity].insert(std::pair<std::string, Component*>("MeshSystem", mesh));

	Component* spatial = m_systems["SpatialSystem"]->create();
	spatial->m_entity = entity;	
	m_entities[entity].insert(std::pair<std::string, Component*>("SpatialSystem", spatial));
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