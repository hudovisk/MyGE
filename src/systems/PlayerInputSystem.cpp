
#include "systems/PlayerInputSystem.h"

#include "core/Engine.h"
#include "debug/Logger.h"
#include "debug/MyAssert.h"

PlayerInputSystem::PlayerInputSystem()
	: m_isInitialised(false)
{

}

PlayerInputSystem::~PlayerInputSystem()
{
	destroy();
}

bool PlayerInputSystem::init()
{
	if(m_isInitialised)
		return false;

	if(!m_componentPool.init(10))
	{
		LOG(ERROR, "Couldnt initialise m_componentPool with 10 objects.");
		return false;
	}

	m_isInitialised = true;

	return true;
}

bool PlayerInputSystem::destroy()
{
	if(m_isInitialised)
	{
		m_componentPool.destroy();
	}

	UpdateStageEventData event;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<PlayerInputSystem,&PlayerInputSystem::onUpdate>(this),
		event.getType());

	return true;
}

void PlayerInputSystem::onUpdate(IEventDataPtr e)
{
	InputComponent** components = m_componentPool.getUsedBufferCache();
	unsigned int numComponents = m_componentPool.getUsedSize();

	std::list<InputEvent> inputEvents = Engine::g_eventManager.getInputQueue();

	for(unsigned int i=0; i<numComponents; i++)
	{
		unsigned int numMessages = parse(inputEvents, components[i]);
		for(unsigned int j=0; j<numMessages; j++)
		{
			Engine::g_entityManager.sendMessage(IMessageDataPtr(m_messageBuffer[j]));
		}
	}
}

Component* PlayerInputSystem::createFromJSON(const char* json)
{
	using namespace rapidxml;

	InputComponent* inputComponent = m_componentPool.create();

	inputComponent->m_keyMap.clear();
	inputComponent->m_mouseMap.clear();

	FILE* file = fopen(json, "rb");
	ASSERT(file != nullptr, "Couldn't open file: "<<json);
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);

	fseek(file, 0L, SEEK_SET);

	char* buffer = (char*) calloc(size,sizeof(char));
	if(!buffer)
	{
		// LOG(ERROR, "File: "<<json<<" too big");
		fclose(file);

		ASSERT(false,"File: "<<json<<" too big");
	}

	fread(buffer, 1, size, file);

	xml_document<> doc;
	try
	{
		doc.parse<0>(buffer);
	}
	catch(parse_error& e)
	{
		fclose(file);
		free(buffer);

		ASSERT(false,"InputContext init file: "<<json<<" error, possibly wrong file or bad XML. parser error: "<<e.what());
	}

	xml_node<>* root = doc.first_node();

	xml_node<>* node = root->first_node();
	while(node)
	{
		std::string name = node->name();

		if(name == "message")
		{
			unsigned int messageId = atoi(node->first_attribute("id")->value());
			xml_node<>* childNode = node->first_node();
			while(childNode)
			{
				std::string childName = childNode->name();
				if(childName == "key")
				{
				  KeyboardEvent keyEvent = mapXmlKeyNode(childNode);
				  inputComponent->m_keyMap[keyEvent].push_back(messageId);
				}
				else if(childName == "mouse")
				{
				  MouseMapAttributes mouseAttributes = mapXmlMouseNode(childNode);
				  mouseAttributes.m_messageId = messageId;
				  inputComponent->m_mouseMap[mouseAttributes.m_mouseCode].push_back(mouseAttributes);
				}
				else
				{
				  ASSERT(false,"InputContext init file: "<<json<<" error, node name: "<<name<<" not identified.");
				}
				childNode = childNode->next_sibling();
			}
		}
		else
		{
		  ASSERT(false,"InputContext init file: "<<json<<" error, node name: "<<name<<" not identified.");
		}
		node = node->next_sibling();
	}

	free(buffer);
	fclose(file);

	return inputComponent;
}

Component* PlayerInputSystem::create()
{
	InputComponent* inputComponent = m_componentPool.create();
	return inputComponent;
}

void PlayerInputSystem::release(Component* inputComponent)
{
	m_componentPool.release(dynamic_cast<InputComponent*>(inputComponent));
}

unsigned int PlayerInputSystem::parse(std::list<InputEvent>& input, 
	InputComponent* inputComponent)
{
	int numMessages = 0;
	for(auto itInput = input.begin(); itInput != input.end(); ++itInput)
	{
		switch(itInput->m_type)
		{
			case InputEventType::KEYBOARD :
			{
				auto itFind = inputComponent->m_keyMap.find(itInput->m_key);
				if(itFind != inputComponent->m_keyMap.end())
				{
					for(unsigned int i=0; i < itFind->second.size(); i++)
					{
						m_messageBuffer[numMessages]->setId(itFind->second.at(i));
					  	numMessages++;
					}
				}
				break;
			}
 			case InputEventType::MOUSE :
 			{
 				auto itFind = inputComponent->m_mouseMap.find(itInput->m_mouse.m_code);	
 				if(itFind != inputComponent->m_mouseMap.end())
 				{
					for(auto itAtributes = itFind->second.begin(); itAtributes != itFind->second.end(); itAtributes++)
					{
						MouseMapAttributes mouseAttributes = *itAtributes;
						if(itInput->m_mouse.m_pressed == mouseAttributes.m_pressed)
						{
							m_messageBuffer[numMessages]->setId(mouseAttributes.m_messageId);
							int posX = (mouseAttributes.m_relativeX) ? itInput->m_mouse.m_relX : itInput->m_mouse.m_posX;
							int posY = (mouseAttributes.m_relativeY) ? itInput->m_mouse.m_relY : itInput->m_mouse.m_posY;
							m_messageBuffer[numMessages]->setInputX((mouseAttributes.m_invertedX) ? -posX : posX);
							m_messageBuffer[numMessages]->setInputY((mouseAttributes.m_invertedY) ? -posY : posY);
							numMessages++;
						}
					}
				}
				break;
			}
			default:
			{			
				break;
			}
		}
	}
	return numMessages;
}

KeyboardEvent PlayerInputSystem::mapXmlKeyNode(rapidxml::xml_node<>* node)
{
	KeyboardEvent keyEvent;
	
	if(strcmp(node->first_attribute("value")->value(), "F1") == 0)
		keyEvent.m_code = SDLK_F1;
	else if(strcmp(node->first_attribute("value")->value(), "F2") == 0)
		keyEvent.m_code = SDLK_F2;
	else if(strcmp(node->first_attribute("value")->value(), "F3") == 0)
		keyEvent.m_code = SDLK_F3;
	else if(strcmp(node->first_attribute("value")->value(), "F4") == 0)
		keyEvent.m_code = SDLK_F4;
	else if(strcmp(node->first_attribute("value")->value(), "F5") == 0)
		keyEvent.m_code = SDLK_F5;
	else if(strcmp(node->first_attribute("value")->value(), "F6") == 0)
		keyEvent.m_code = SDLK_F6;
	else if(strcmp(node->first_attribute("value")->value(), "F7") == 0)
		keyEvent.m_code = SDLK_F7;
	else if(strcmp(node->first_attribute("value")->value(), "F8") == 0)
		keyEvent.m_code = SDLK_F8;
	else if(strcmp(node->first_attribute("value")->value(), "F9") == 0)
		keyEvent.m_code = SDLK_F9;
	else if(strcmp(node->first_attribute("value")->value(), "F10") == 0)
		keyEvent.m_code = SDLK_F10;
	else if(strcmp(node->first_attribute("value")->value(), "F11") == 0)
		keyEvent.m_code = SDLK_F11;
	else if(strcmp(node->first_attribute("value")->value(), "F12") == 0)
		keyEvent.m_code = SDLK_F12;

	else if(strcmp(node->first_attribute("value")->value(), "ESC") == 0)
		keyEvent.m_code = SDLK_ESCAPE;
	else	
		keyEvent.m_code = tolower(*(node->first_attribute("value")->value()));

	keyEvent.m_pressed = (strcmp(node->first_attribute("pressed")->value(),"true") == 0) ? true : false;

	return keyEvent;
}

MouseMapAttributes PlayerInputSystem::mapXmlMouseNode(rapidxml::xml_node<>* node)
{
	MouseMapAttributes mouseAttributes;
	unsigned int mouseCode = 0;
	if(strcmp(node->first_attribute("value")->value(), "MOTION") == 0)
	{
		mouseCode = MOUSE_MOTION;
	}
	else if(strcmp(node->first_attribute("value")->value(), "BUTTON_LEFT") == 0)
	{
		mouseCode = MOUSE_BUTTON_LEFT;
	}
	else if(strcmp(node->first_attribute("value")->value(), "BUTTON_RIGHT") == 0)
	{
		mouseCode = MOUSE_BUTTON_RIGHT;
	}
	else if(strcmp(node->first_attribute("value")->value(), "BUTTON_MIDDLE") == 0)
	{
		mouseCode = MOUSE_BUTTON_MIDDLE;
	}
	else if(strcmp(node->first_attribute("value")->value(), "WHEEL_UP") == 0)
	{
		mouseCode = MOUSE_WHEEL_UP;
	}
	else if(strcmp(node->first_attribute("value")->value(), "WHEEL_DOWN") == 0)
	{
		mouseCode = MOUSE_WHEEL_DOWN;
	}
	else
	{
		ASSERT(false, "Incorrect mouse value: "<<node->first_attribute("value")->value());
	}
	mouseAttributes.m_mouseCode = mouseCode;
	
	mouseAttributes.m_pressed = (strcmp(node->first_attribute("pressed")->value(),"true") == 0) ? true : false;

	//mouseAttributes.m_messageCode = atoi(node->first_attribute("message")->value());

	rapidxml::xml_node<>* nodeAxis = node->first_node("x");
	mouseAttributes.m_invertedX = (strcmp(nodeAxis->first_attribute("inverted")->value(),"true") == 0);
	mouseAttributes.m_relativeX = (strcmp(nodeAxis->first_attribute("relative")->value(),"true") == 0);
	
	nodeAxis = node->first_node("y");
	mouseAttributes.m_invertedY = (strcmp(nodeAxis->first_attribute("inverted")->value(),"true") == 0);
	mouseAttributes.m_relativeY = (strcmp(nodeAxis->first_attribute("relative")->value(),"true") == 0);

	return mouseAttributes;
}