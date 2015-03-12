
#include "core/EntityManager.h"

#include "systems/MeshSystem.h"
#include "systems/SpatialSystem.h"
#include "systems/InputSystem.h"
#include "systems/MovementSystem.h"
#include "systems/PlayerControllerSystem.h"
#include "systems/CameraSystem.h"
#include "systems/LightSystem.h"

#include "debug/MyAssert.h"
#include "core/Engine.h"



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
	m_systems["CameraSystem"] = new CameraSystem();
	m_systems["LightSystem"] = new LightSystem();

	for(auto itSystem = m_systems.begin(); itSystem != m_systems.end(); itSystem++)
	{
		if(!itSystem->second->init())
		{
			LOG(ERROR, "System: "<<itSystem->first<<" failed to initialise.");
			return false;
		}
	}

	char* buffer = getFileBuffer("res/scripts/config.json");
	rapidjson::Document doc;
	doc.Parse<0>(buffer);

	ASSERT(doc.IsObject(), "Wrong JSON format, config.json must be an object.");
	ASSERT(doc.HasMember("entities"), "Wrong JSON format, config.json must have entities member.");

	loadEntities(doc["entities"].GetString());

	for(auto itConfig = doc.MemberBegin(); 
		itConfig != doc.MemberEnd(); itConfig++)
	{
		if(strcmp("defaultCamera", itConfig->name.GetString()) == 0)
		{
			auto itFind = m_entitiesNames.find(itConfig->value.GetString());
			ASSERT(itFind != m_entitiesNames.end(), "defaultCamera value doest not exists.");

			auto itComponent = m_entities[itFind->second].find("CameraSystem");
			ASSERT(itComponent != m_entities[itFind->second].end(), "defaultCamera does not have a camera component.");

			Engine::g_renderManager.setDefaultCamera(
				dynamic_cast<CameraComponent*> (itComponent->second));
		}
	}
	free(buffer);

	m_isInitialised = true;

	return true;
}

bool EntityManager::destroy()
{
	if(m_isInitialised)
	{
		LOG(INFO, "Destroying Systems.");
		//RELEASE COMPONENTS
		for(auto itEntity = m_entities.begin(); itEntity != m_entities.end(); itEntity++)
		{
			for(auto itComponent = itEntity->second.begin(); 
				itComponent != itEntity->second.end(); itComponent++)
			{
				LOG(INFO, "RELEASING COMPONENT FROM SYSTEM: "<<itComponent->first<<" FROM ENTITY: "<<itEntity->first);
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

char* EntityManager::getFileBuffer(std::string filePath)
{
	FILE* file = fopen(filePath.c_str(), "rb");
	ASSERT(file != nullptr, "Couldn't open file: "<<filePath);
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);

	fseek(file, 0L, SEEK_SET);

	char* buffer = (char*) calloc(size,sizeof(char));
	if(!buffer)
	{
		// LOG(ERROR, "File: "<<json<<" too big");
		fclose(file);

		ASSERT(false,"File: "<<filePath<<" too big");
	}
	fread(buffer, 1, size, file);
	fclose(file);

	return buffer;
}

void EntityManager::loadEntities(std::string filePath)
{
	char* buffer = getFileBuffer(filePath);

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
				LOG(INFO, "Loading entity id: "<<entity<<" Name: "<<itMember->value.GetString());
				m_entitiesNames[itMember->value.GetString()] = entity;
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