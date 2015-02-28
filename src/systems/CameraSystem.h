
#ifndef CAMERA_SYSTEM_H
#define CAMERA_SYSTEM_H

#include "systems/System.h"

#include "core/Messages.h"
#include "memory/ObjectPool.h"
#include "events/Events.h"

class CameraSystem;

const float PIdiv180 = 0.017453;

class CameraComponent : public Component
{
public:
	CameraComponent() { }
	~CameraComponent() { }
	
	void receiveMessage(IMessageDataPtr message);

private:
	friend class CameraSystem;
	friend class RenderManager;

	CameraSystem* m_system;

	float m_fov;
	float m_nearDistance;
	float m_farDistance;
	float m_aspectRatio;
	float m_yaw;
	float m_pitch;
	float m_sensibility;
	bool m_dirty;
	Matrix4 m_projectionMatrix;
};

class CameraSystem : public System
{
public:
	CameraSystem();
	~CameraSystem();

	bool init();
	bool destroy();

	Component* create();
	Component* createFromJSON(const rapidjson::Value& jsonObject);

	void release(Component* component);

	void receiveMessage(CameraComponent* component, IMessageDataPtr message);
	
	void onWindowResize(IEventDataPtr);

private:
	CameraComponent* getComponentByEntity(EntityHandler entity);
	enum {
		START_MOVE_FORWARD = 0x01,
		STOP_MOVE_FORWARD_BACKWARD = 0x02,
		LOOK_MOVEMENT = 0X03,
		START_MOVE_LEFT = 0x04,
		START_MOVE_RIGHT = 0x05,
		STOP_MOVE_LEFT_RIGHT = 0x06,
		START_MOVE_BACKWARD = 0x07,
	};

	bool m_isInitialised;
	unsigned int m_inputMessageType;

	MovementMessage m_movementMessage;
	GetTransformMessage m_getTransformMessage;

	ObjectPool<CameraComponent> m_componentPool;
};

#endif //CAMERA_SYSTEM_H