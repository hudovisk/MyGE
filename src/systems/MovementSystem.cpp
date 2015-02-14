
#include "systems/MovementSystem.h"

#include "core/Engine.h"

void MovementComponent::receiveMessage(IMessageDataPtr message)
{
	m_system->receiveMessage(this, message);
}

MovementSystem::MovementSystem()
	: m_isInitialised(false), m_movementMessageType(0)
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

	if(!m_componentPool.init(100))
	{
		LOG(ERROR, "Could not initialise m_componentPool with 10 ojects.");
		return false;
	}

	UpdateStageEventData event;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<MovementSystem,&MovementSystem::onUpdate>(this),
		event.getType());

	MovementMessage message;
	m_movementMessageType = message.getType();

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
	float dt = ((UpdateStageEventData*) e.get())->getDeltaT();

	MovementComponent** components = m_componentPool.getUsedBufferCache();
	unsigned int numComponents = m_componentPool.getUsedSize();

	for(unsigned int i=0; i<numComponents; i++)
	{
		MovementComponent* component = components[i];

		if(component->m_currentVelocity == 0)
			continue;

		m_getTransformMsg.setHandled(false);
		m_getTransformMsg.setEntityHandler(component->m_entity);
		Engine::g_entityManager.sendMessage(&m_getTransformMsg);

		if(!m_getTransformMsg.isHandled())
		{
			LOG(ERROR, "Entity id: "<<component->m_entity<<" does not have a transform component.");
			return;
		}
		Transform* transform = m_getTransformMsg.getTransform();


		Vec3 dir = transform->transform(Vec4(component->m_localDirection, 0));

		Vec3 pos = transform->getPosition() + dir*component->m_currentVelocity*dt;

		transform->translate( pos );
	}
}

void MovementSystem::receiveMessage(MovementComponent* component,
	IMessageDataPtr message)
{
	if(message->getType() == m_movementMessageType)
	{
		message->setHandled(true);
		MovementMessage* movementMessage = (MovementMessage*) message;

		movementMessage->m_currentVelocity = &component->m_currentVelocity;
		movementMessage->m_maxVelocity = &component->m_maxVelocity;
		movementMessage->m_localDirection = &component->m_localDirection;
	}
}

Component* MovementSystem::create()
{
	MovementComponent* component = m_componentPool.create();
	component->m_system = this;

	component->m_maxVelocity = 1;
	component->m_currentVelocity = 0;
	component->m_localDirection = Vec3(0,0,0);

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

