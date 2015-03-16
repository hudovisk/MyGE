
#include "systems/CameraSystem.h"

#include "core/Engine.h"

void CameraComponent::receiveMessage(IMessageDataPtr message)
{
	m_system->receiveMessage(this, message);
}

CameraSystem::CameraSystem()
	: m_isInitialised(false), m_inputMessageType(0)
{

}

CameraSystem::~CameraSystem()
{
	destroy();
}

bool CameraSystem::init()
{
	if(!m_componentPool.init(10))
	{
		LOG(ERROR, "Couldnt initialise m_componentPool with 10 objects.");
		return false;
	}

	InputMessage inputMessage;
	m_inputMessageType = inputMessage.getType();

	WindowResizedEventData windowResizedEventData;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method
				<CameraSystem,&CameraSystem::onWindowResize>(this),
		windowResizedEventData.getType());

	m_isInitialised = true;
	return true;
}

bool CameraSystem::destroy()
{
	if(m_isInitialised)
	{
		LOG(INFO, "Destroying CameraSystem.");
		m_componentPool.destroy();

		WindowResizedEventData windowResizedEventData;
		Engine::g_eventManager.removeListenner(
			EventListenerDelegate::from_method
					<CameraSystem,&CameraSystem::onWindowResize>(this),
			windowResizedEventData.getType());

		m_isInitialised = false;
	}

	return true;
}

Component* CameraSystem::create()
{
	CameraComponent* component = m_componentPool.create();
	component->m_system = this;

	component->m_fov = 90 * PIdiv180;
	component->m_nearDistance = 1;
	component->m_farDistance = 10;
	component->m_aspectRatio = 1;
	component->m_yaw = 0;
	component->m_pitch = 0;
	component->m_dirty = true;

	return component;
}

Component* CameraSystem::createFromJSON(const rapidjson::Value& jsonObject)
{
	CameraComponent* component = m_componentPool.create();
	component->m_system = this;

	component->m_yaw = 0;
	component->m_pitch = 0;
	component->m_dirty = true;

	for(auto itMember = jsonObject.MemberBegin();
		itMember != jsonObject.MemberEnd(); itMember++)
	{
		if(strcmp("fov", itMember->name.GetString()) == 0)
		{
			component->m_fov = itMember->value.GetDouble() * PIdiv180;
		}
		else if(strcmp("nearDistance", itMember->name.GetString()) == 0)
		{
			component->m_nearDistance = itMember->value.GetDouble();
		}
		else if(strcmp("farDistance", itMember->name.GetString()) == 0)
		{
			component->m_farDistance = itMember->value.GetDouble();
		}
		else if(strcmp("aspectRatio", itMember->name.GetString()) == 0)
		{
			component->m_aspectRatio = itMember->value.GetDouble();
		}
		else if(strcmp("sensibility", itMember->name.GetString()) == 0)
		{
			component->m_sensibility = itMember->value.GetDouble();
		}
	}

	float width = Engine::g_renderManager.getWidth();
	float height = Engine::g_renderManager.getHeight();
	component->m_aspectRatio = width/height;
	
	return component;
}

void CameraSystem::release(Component* component)
{
	CameraComponent* camera = dynamic_cast<CameraComponent*>(component);
	camera->m_system = nullptr;
	m_componentPool.release(camera);
}

void CameraSystem::receiveMessage(CameraComponent* component, IMessageDataPtr message)
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
			case LOOK_MOVEMENT:
			{
				//GET ENTITY TRANSFORM
				m_getTransformMessage.setEntityHandler(component->m_entity);
				Engine::g_entityManager.sendMessage(&m_getTransformMessage);

				if(!m_getTransformMessage.isHandled())
				{
					LOG(ERROR, "Entity id: "<<component->m_entity<<
						" does not have a transform component.");
					return;
				}
				Transform* transform = m_getTransformMessage.getTransform();
				CameraComponent* camera = getComponentByEntity(component->m_entity);

				camera->m_yaw = (int) ( (camera->m_yaw + 
					inputMessage->getInputX() * camera->m_sensibility) * 1000)
					 % 3141 * 0.001;
				float tempPitch = camera->m_pitch + 
					inputMessage->getInputY() * camera->m_sensibility;
				if(fabs(tempPitch) < 0.7853) //pi/4
					camera->m_pitch = tempPitch;

				Vec3 globalPitchAxis(1,0,0);
				Quaternion qPitch(cos(camera->m_pitch), 
						globalPitchAxis * sin(camera->m_pitch));

				Vec3 localYawAxis = (qPitch * Quaternion(0,Vec3(0,1,0)) * 
					qPitch.getInverse()).m_axis;
				Quaternion qYaw(cos(camera->m_yaw), localYawAxis * sin(camera->m_yaw));

				transform->setRotation(qYaw * qPitch);
	
			// std::cout<<"Degree yaw: "<<yaw * 114.591559026165 <<" Degree pitch: "<<pitch * 114.591559026165 <<std::endl;
			}
			break;
			case START_MOVE_FORWARD:
			{
				inputMessage->setHandled(true);
				m_movementMessage.setCurrentVelocity( m_movementMessage.getMaxVelocity() );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[2] = -1;
				if(direction.magnitudeSquared() != 0)
					direction.normalize();
				m_movementMessage.setLocalDirection( direction );
			}
			break;
			case START_MOVE_BACKWARD:
			{
				inputMessage->setHandled(true);
				
				m_movementMessage.setCurrentVelocity( m_movementMessage.getMaxVelocity() );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[2] = 1;
				if(direction.magnitudeSquared() != 0)
					direction.normalize();
				m_movementMessage.setLocalDirection( direction );
			}
			break;
			case STOP_MOVE_FORWARD_BACKWARD:
			{
				inputMessage->setHandled(true);
				
				m_movementMessage.setCurrentVelocity( 0 );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[2] = 0;
				m_movementMessage.setLocalDirection( direction );
			}
			break;
			case START_MOVE_LEFT:
			{
				inputMessage->setHandled(true);
				
				m_movementMessage.setCurrentVelocity( m_movementMessage.getMaxVelocity() );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[0] = -1;
				if(direction.magnitudeSquared() != 0)
					direction.normalize();
				m_movementMessage.setLocalDirection( direction );
			}
			break;
			case START_MOVE_RIGHT:
			{
				inputMessage->setHandled(true);
				
				m_movementMessage.setCurrentVelocity( m_movementMessage.getMaxVelocity() );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[0] = 1;
				if(direction.magnitudeSquared() != 0)
					direction.normalize();
				m_movementMessage.setLocalDirection( direction );
			}
			break;
			case STOP_MOVE_LEFT_RIGHT:
			{
				inputMessage->setHandled(true);
				
				m_movementMessage.setCurrentVelocity( 0 );
				direction = m_movementMessage.getLocalDirection();
				direction.m_data[0] = 0;
				m_movementMessage.setLocalDirection( direction );
			}
			break;
		}
	}
}

CameraComponent* CameraSystem::getComponentByEntity(EntityHandler entity)
{
	CameraComponent** components = m_componentPool.getUsedBufferCache();
	unsigned int numComponents = m_componentPool.getUsedSize();

	for(unsigned int i=0; i < numComponents; i++)
	{
		if(components[i]->m_entity == entity)
		{
			return components[i];
		}
	}

	return nullptr;
}

void CameraSystem::onWindowResize(IEventDataPtr e)
{
	WindowResizedEventData* event = dynamic_cast<WindowResizedEventData*>(e.get());
	float aspectRatio = event->getWidth() / event->getHeight();

	CameraComponent** components = m_componentPool.getUsedBufferCache();
	unsigned int numComponents = m_componentPool.getUsedSize();

	for(unsigned int i=0; i<numComponents; i++)
	{
		components[i]->m_aspectRatio = aspectRatio;
		components[i]->m_dirty = true;
	}
}