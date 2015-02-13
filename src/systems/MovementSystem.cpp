
#include "systems/MovementSystem.h"

#include "core/Engine.h"

void MovementComponent::receiveMessage(IMessageDataPtr message)
{
	m_system->receiveMessage(this, message);
}

MovementSystem::MovementSystem()
	: m_isInitialised(false)
{

}

MovementSystem::~MovementSystem()
{
	destroy();
}

bool MovementSystem::init()
{
	if(m_isInitialised)
		return false;

	if(!m_componentPool.init(10))
	{
		LOG(ERROR, "Could not initialise m_componentPool with 10 ojects.");
		return false;
	}

	UpdateStageEventData event;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<MovementSystem,&MovementSystem::onUpdate>(this),
		event.getType());

	return true;
}

bool MovementSystem::destroy()
{
	if(m_isInitialised)
	{
		m_componentPool.destroy();
		m_isInitialised = false;
	}

	return true;
}

void MovementSystem::onUpdate(IEventDataPtr e)
{

}

void MovementSystem::receiveMessage(MovementComponent* component,
	IMessageDataPtr message)
{

}

Component* MovementSystem::create()
{
	MovementComponent* component = m_componentPool.create();
	return component;
}

Component* MovementSystem::createFromJSON(const char* json)
{
	return nullptr;
}

void MovementSystem::release(Component* movementComponent)
{
	m_componentPool.release(dynamic_cast<MovementComponent*>(movementComponent));
}

