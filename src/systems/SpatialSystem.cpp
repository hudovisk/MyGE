
#include "systems/SpatialSystem.h"

#include <iostream>

void SpatialComponent::receiveMessage(IMessageDataPtr message)
{
	m_system->receiveMessage(this, message);
}

SpatialSystem::SpatialSystem()
	: m_isInitialised(false), m_getTransformMsg(nullptr)
{

}

SpatialSystem::~SpatialSystem()
{
	destroy();
}

bool SpatialSystem::init()
{
	if(m_isInitialised)
		return false;

	if(!m_componentPool.init(100))
	{
		LOG(ERROR, "Couldnt initialise m_componentPool with 100 objects.");
		return false;
	}

	m_getTransformMsg = std::shared_ptr<GetTransformMessage>(new GetTransformMessage());

	m_isInitialised = true;

	return true;
}

bool SpatialSystem::destroy()
{
	if(m_isInitialised)
	{
		m_componentPool.destroy();
		delete m_getTransformMsg.get();
	}

	return true;
}

void SpatialSystem::receiveMessage(SpatialComponent* component, IMessageDataPtr message)
{
	if(message->getType() == m_getTransformMsg->getType())
	{
		GetTransformMessage* getTransformMsg = dynamic_cast<GetTransformMessage*>(message.get());
		getTransformMsg->setTransform(&component->m_transform);
		getTransformMsg->setHandled(true);
	}
}

Component* SpatialSystem::create()
{
	SpatialComponent* spatial = m_componentPool.create();
	spatial->m_system = this;
	return spatial;
}


Component* SpatialSystem::createFromJSON(const char* json)
{
	SpatialComponent* spatial = m_componentPool.create();
	spatial->m_system = this;
	return spatial;
}

void SpatialSystem::release(Component* component)
{
	LOG(INFO, "SpatialSystem release");
	SpatialComponent* spatial = dynamic_cast<SpatialComponent*>(component);

	spatial->m_system = nullptr;

	m_componentPool.release(spatial);
}