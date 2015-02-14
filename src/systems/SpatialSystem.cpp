
#include "systems/SpatialSystem.h"

#include <iostream>

void SpatialComponent::receiveMessage(IMessageDataPtr message)
{
	m_system->receiveMessage(this, message);
}

SpatialSystem::SpatialSystem()
	: m_isInitialised(false)
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

	m_isInitialised = true;

	return true;
}

bool SpatialSystem::destroy()
{
	if(m_isInitialised)
	{
		m_componentPool.destroy();
	}

	return true;
}

void SpatialSystem::receiveMessage(SpatialComponent* component, IMessageDataPtr message)
{
	if(message->getType() == m_getTransformMsg.getType())
	{
		GetTransformMessage* getTransformMsg = dynamic_cast<GetTransformMessage*>(message);
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


Component* SpatialSystem::createFromJSON(const rapidjson::Value& jsonObject)
{
	SpatialComponent* spatial = m_componentPool.create();
	spatial->m_system = this;

	for(auto itMember = jsonObject.MemberBegin();
		itMember != jsonObject.MemberEnd(); itMember++)
	{
		if(strcmp("pos", itMember->name.GetString()) == 0)
		{
			Vec3 pos;
			pos.m_data[0] = itMember->value[0u].GetDouble();
			pos.m_data[1] = itMember->value[1].GetDouble();
			pos.m_data[2] = itMember->value[2].GetDouble();
			spatial->m_transform.translate(pos);
		}
		else if(strcmp("scale", itMember->name.GetString()) == 0)
		{
			Vec3 scale;
			scale.m_data[0] = itMember->value[0u].GetDouble();
			scale.m_data[1] = itMember->value[1].GetDouble();
			scale.m_data[2] = itMember->value[2].GetDouble();
			spatial->m_transform.translate(scale);	
		}
	}

	return spatial;
}

void SpatialSystem::release(Component* component)
{
	SpatialComponent* spatial = dynamic_cast<SpatialComponent*>(component);

	spatial->m_system = nullptr;

	m_componentPool.release(spatial);
}