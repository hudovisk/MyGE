
#include "systems/PlayerControllerSystem.h"

#include "core/Engine.h"

void PlayerComponent::receiveMessage(IMessageDataPtr message)
{
	m_system->receiveMessage(this, message);
}

PlayerControllerSystem::PlayerControllerSystem()
	: m_isInitialised(false), m_inputMessageType(0)
{

}
PlayerControllerSystem::~PlayerControllerSystem()
{
	destroy();
}

bool PlayerControllerSystem::init()
{
	if(m_isInitialised)
		return false;

	if(!m_componentPool.init(100))
	{
		LOG(ERROR, "Could not initialise m_componentPool with 10 ojects.");
		return false;
	}

	InputMessage inputMessage;
	m_inputMessageType = inputMessage.getType();

	m_isInitialised = true;
	return true;
}
	
bool PlayerControllerSystem::destroy()
{
	if(m_isInitialised)
	{
		LOG(INFO, "Destroying PlayerControllerSystem.");
		m_componentPool.destroy();
		
		m_isInitialised = false;
	}

	return true;
}

void PlayerControllerSystem::receiveMessage(PlayerComponent* component, IMessageDataPtr message)
{
	if(message->getType() == m_inputMessageType)
	{
		InputMessage* inputMessage = (InputMessage*) message;

		m_movementMessage.setEntityHandler(component->m_entity);
		m_movementMessage.setHandled(false);
		Engine::g_entityManager.sendMessage(&m_movementMessage);
		if(!m_movementMessage.isHandled())
		{
			LOG(INFO, "Entity of id: "<<component->m_entity<<
				" does not have a MovementComponent.");
			return;
		}

		Vec3 direction;
		switch(inputMessage->getId())
		{
			case START_MOVE_FORWARD:
				inputMessage->setHandled(true);
				LOG(INFO, "START_MOVE_FORWARD");
				m_movementMessage.setCurrentVelocity( m_movementMessage.getMaxVelocity() );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[1] = 1;
				if(direction.magnitudeSquared() != 0)
					direction.normalize();
				m_movementMessage.setLocalDirection( direction );
			break;
			case START_MOVE_BACKWARD:
				inputMessage->setHandled(true);
				LOG(INFO, "START_MOVE_BACKWARD");
				m_movementMessage.setCurrentVelocity( m_movementMessage.getMaxVelocity() );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[1] = -1;
				if(direction.magnitudeSquared() != 0)
					direction.normalize();
				m_movementMessage.setLocalDirection( direction );
			break;
			case STOP_MOVE_FORWARD_BACKWARD:
				inputMessage->setHandled(true);
				LOG(INFO, "STOP_MOVE_FORWARD_BACKWARD");
				m_movementMessage.setCurrentVelocity( 0 );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[1] = 0;
				m_movementMessage.setLocalDirection( direction );
			break;
			case START_MOVE_LEFT:
				inputMessage->setHandled(true);
				LOG(INFO, "START_MOVE_LEFT");
				m_movementMessage.setCurrentVelocity( m_movementMessage.getMaxVelocity() );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[0] = 1;
				if(direction.magnitudeSquared() != 0)
					direction.normalize();
				m_movementMessage.setLocalDirection( direction );
			break;
			case START_MOVE_RIGHT:
				inputMessage->setHandled(true);
				LOG(INFO, "START_MOVE_RIGHT");
				m_movementMessage.setCurrentVelocity( m_movementMessage.getMaxVelocity() );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[0] = -1;
				if(direction.magnitudeSquared() != 0)
					direction.normalize();
				m_movementMessage.setLocalDirection( direction );
			break;
			case STOP_MOVE_LEFT_RIGHT:
				inputMessage->setHandled(true);
				LOG(INFO, "STOP_MOVE_LEFT_RIGHT");
				m_movementMessage.setCurrentVelocity( 0 );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[0] = 0;
				m_movementMessage.setLocalDirection( direction );
			break;
		}
	}
}

Component* PlayerControllerSystem::create()
{
	PlayerComponent* component = m_componentPool.create();
	component->m_system = this;
	return component;
}
	
Component* PlayerControllerSystem::createFromJSON(const rapidjson::Value& jsonObject)
{
	PlayerComponent* component = m_componentPool.create();
	component->m_system = this;
	return component;
}

void PlayerControllerSystem::release(Component* component)
{
	PlayerComponent* playerComponent = dynamic_cast<PlayerComponent*>(component);
	playerComponent->m_system = nullptr;
	m_componentPool.release(playerComponent);
}