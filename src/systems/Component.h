
#ifndef COMPONENT_H
#define COMPONENT_H

#include "core/Messages.h"

#include "debug/Logger.h"

enum ComponentType
{
	MESH_COMPONENT,
	SPATIAL_COMPONENT
};

class Component
{
public:
	Component() { }
	virtual ~Component() { }
	
	virtual void receiveMessage(IMessageDataPtr message) { }
	

protected:
	friend class EntityManager;

	EntityHandler m_entity;
};

#endif