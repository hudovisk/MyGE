
#ifndef PLAYER_INPUT_SYSTEM_H
#define PLAYER_INPUT_SYSTEM_H

#include "core/Messages.h"
#include "systems/Component.h"
#include "memory/ObjectPool.h"
#include "events/InputTypes.h"
#include "events/Events.h"

#include "rapidxml-1.13/rapidxml.hpp"

#include <map>
#include <vector>
#include <list>

class PlayerInputSystem;

class InputComponent : public Component
{
public:
	InputComponent() { }
	virtual ~InputComponent() { }

private:

	friend class PlayerInputSystem;

	using MouseMap = std::pair<unsigned int, std::vector<MouseMapAttributes>>;
	using KeyMap = std::pair<KeyboardEvent, std::vector<unsigned int>>;

	std::map<KeyboardEvent, std::vector<unsigned int>> m_keyMap;
	std::map<unsigned int, std::vector<MouseMapAttributes>> m_mouseMap;
};

class PlayerInputSystem
{
public:
	PlayerInputSystem();
	~PlayerInputSystem();

	bool init();
	bool destroy();

	void onUpdate(IEventDataPtr e);

	InputComponent* createInputFromFile(const char* filePath);
	InputComponent* createInput();

	void releaseInput(InputComponent* inputComponent);
	
private:
	unsigned int parse(std::list<InputEvent>& input, InputComponent* inputComponent);

	KeyboardEvent mapXmlKeyNode(rapidxml::xml_node<>* node);
	MouseMapAttributes mapXmlMouseNode(rapidxml::xml_node<>* node);

	std::shared_ptr<PlayerInputMessage> m_messageBuffer[10];
	ObjectPool<InputComponent> m_inputPool;

	bool m_isInitialised;
};

#endif //PLAYER_INPUT_SYSTEM_H