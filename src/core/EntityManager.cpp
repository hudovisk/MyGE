
#include "core/EntityManager.h"

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
	if(!m_meshSystem.init())
	{
		return false;
	}
	if(!m_spatialSystem.init())
	{
		return false;
	}

	m_isInitialised = true;

	return true;
}

bool EntityManager::destroy()
{
	if(m_isInitialised)
	{
		m_spatialSystem.destroy();
		m_meshSystem.destroy();			
	}	
	return true;
}

void EntityManager::loadEntity(const char* filePath)
{
	EntityHandler entity = EntityManager::NEXT_ID++;

	MeshComponent* mesh = m_meshSystem.createMeshFromFile(filePath);
	mesh->m_entity = entity;

	SpatialComponent* spatial = m_spatialSystem.createSpatial();
	spatial->m_entity = entity;

	m_entities[entity].insert(std::pair<ComponentType, Component*>(MESH_COMPONENT, mesh));
	m_entities[entity].insert(std::pair<ComponentType, Component*>(SPATIAL_COMPONENT, spatial));
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