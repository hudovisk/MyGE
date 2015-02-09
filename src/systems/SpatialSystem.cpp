
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

	if(!m_spatialPool.init(100))
	{
		LOG(ERROR, "Couldnt initialise m_spatialPool with 100 objects.");
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
		m_spatialPool.destroy();
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

SpatialComponent* SpatialSystem::createSpatial()
{
	SpatialComponent* spatial = m_spatialPool.create();
	spatial->m_system = this;
	return spatial;
}

void SpatialSystem::releaseSpatial(SpatialComponent* spatial)
{
	m_spatialPool.release(spatial);
}