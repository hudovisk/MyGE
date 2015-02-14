
#ifndef PLAYER_CONTROLLER_SYSTEM
#define PLAYER_CONTROLLER_SYSTEM

#include "systems/System.h"
#include "core/Messages.h"
#include "memory/ObjectPool.h"

class PlayerControllerSystem;

class PlayerComponent : public Component
{
public:
	PlayerComponent() { }
	~PlayerComponent() { }

	void receiveMessage(IMessageDataPtr message);

private:
	friend class PlayerControllerSystem;

	PlayerControllerSystem* m_system;
};

class PlayerControllerSystem : public System
{
public:
	PlayerControllerSystem();
	~PlayerControllerSystem();

	bool init();
	bool destroy();

	void receiveMessage(PlayerComponent* component, IMessageDataPtr message);

	Component* create();
	Component* createFromJSON(const rapidjson::Value& jsonObject);

	void release(Component* component);
private:

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

	ObjectPool<PlayerComponent> m_componentPool;
};

#endif //PLAYER_CONTROLLER_SYSTEM