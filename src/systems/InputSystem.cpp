
#include "systems/InputSystem.h"

#include "core/Engine.h"
#include "debug/Logger.h"
#include "debug/MyAssert.h"

InputSystem::InputSystem()
	: m_isInitialised(false)
{

}

InputSystem::~InputSystem()
{
	destroy();
}

bool InputSystem::init()
{
	if(m_isInitialised)
		return false;

	if(!m_componentPool.init(100))
	{
		LOG(ERROR, "Couldnt initialise m_componentPool with 10 objects.");
		return false;
	}

	UpdateStageEventData event;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<InputSystem,&InputSystem::onUpdate>(this),
		event.getType());

	m_isInitialised = true;

	return true;
}

bool InputSystem::destroy()
{
	if(m_isInitialised)
	{
		LOG(INFO, "Destroying InputSystem.");
		UpdateStageEventData event;
		Engine::g_eventManager.removeListenner(
			EventListenerDelegate::from_method<InputSystem,&InputSystem::onUpdate>(this),
			event.getType());
		m_componentPool.destroy();

		m_isInitialised = false;
	}

	return true;
}

void InputSystem::onUpdate(IEventDataPtr e)
{
	InputComponent** components = m_componentPool.getUsedBufferCache();
	unsigned int numComponents = m_componentPool.getUsedSize();

	std::list<InputEvent>& inputEvents = Engine::g_eventManager.getInputQueue();
	for(unsigned int i=0; i<numComponents; i++)
	{
		unsigned int numMessages = parse(inputEvents, components[i]);
		for(unsigned int j=0; j<numMessages; j++)
		{
			Engine::g_entityManager.sendMessage(&m_messageBuffer[j]);
		}
	}
}

Component* InputSystem::create()
{
	InputComponent* inputComponent = m_componentPool.create();
	return inputComponent;
}

Component* InputSystem::createFromJSON(const rapidjson::Value& jsonObject)
{
	InputComponent* component = m_componentPool.create();

	for(auto itMember = jsonObject.MemberBegin();
		itMember != jsonObject.MemberEnd(); itMember++)
	{
		if(strcmp("file", itMember->name.GetString()) == 0)
		{
			loadFile(component, itMember->value.GetString());
		}
	}

	return component;
}

void InputSystem::loadFile(InputComponent* inputComponent, 
	const char* filePath)
{
	using namespace rapidxml;

	inputComponent->m_keyMap.clear();
	inputComponent->m_mouseMap.clear();

	FILE* file = fopen(filePath, "rb");
	ASSERT(file != nullptr, "Couldn't open file: "<<filePath);
	fseek(file, 0L, SEEK_END);
	unsigned int size = ftell(file);

	fseek(file, 0L, SEEK_SET);

	char* buffer = new char[size + 1];
	if(!buffer)
	{
		LOG(ERROR, "File: "<<filePath<<" too big");
		fclose(file);

		ASSERT(false,"File: "<<filePath<<" too big");
	}

	fread(buffer, 1, size, file);
	buffer[size] = '\0';

	xml_document<> doc;
	try
	{
		doc.parse<0>(buffer);
	}
	catch(parse_error& e)
	{
		fclose(file);
		free(buffer);

		ASSERT(false,"InputContext init file: "<<filePath<<" error, possibly wrong file or bad XML. parser error: "<<e.what());
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
				  ASSERT(false,"InputContext init file: "<<filePath<<" error, node name: "<<name<<" not identified.");
				}
				childNode = childNode->next_sibling();
			}
		}
		else
		{
		  ASSERT(false,"InputContext init file: "<<filePath<<" error, node name: "<<name<<" not identified.");
		}
		node = node->next_sibling();
	}

	fclose(file);
	delete [] buffer;
}

void InputSystem::release(Component* inputComponent)
{
	m_componentPool.release(dynamic_cast<InputComponent*>(inputComponent));
}

unsigned int InputSystem::parse(std::list<InputEvent>& input, 
	InputComponent* inputComponent)
{
	int numMessages = 0;
	for(auto itInput = input.begin(); itInput != input.end(); itInput++)
	{
		if(numMessages >= 100)
		{
			LOG(WARN, "Message Buffer exceded! skipping messages.");
			return numMessages;
		}
		switch(itInput->m_type)
		{
			case InputEventType::KEYBOARD :
			{
				auto itFind = inputComponent->m_keyMap.find(itInput->m_key);
				if(itFind != inputComponent->m_keyMap.end())
				{
					for(unsigned int i=0; i < itFind->second.size(); i++)
					{
						m_messageBuffer[numMessages].setId(itFind->second.at(i));
						m_messageBuffer[numMessages].setEntityHandler(inputComponent->m_entity);
					  	numMessages++;
					}
					itInput = input.erase(itInput);
					if(itInput == input.end())
						return numMessages;
				}
				break;
			}
 			case InputEventType::MOUSE :
 			{
 				auto itFind = inputComponent->m_mouseMap.find(itInput->m_mouse.m_code);	
 				if(itFind != inputComponent->m_mouseMap.end())
 				{
 					bool handled = false;
					for(auto itAtributes = itFind->second.begin(); itAtributes != itFind->second.end(); itAtributes++)
					{
						MouseMapAttributes mouseAttributes = *itAtributes;
						if(itInput->m_mouse.m_pressed == mouseAttributes.m_pressed)
						{
							m_messageBuffer[numMessages].setId(mouseAttributes.m_messageId);
							int posX = (mouseAttributes.m_relativeX) ? itInput->m_mouse.m_relX : itInput->m_mouse.m_posX;
							int posY = (mouseAttributes.m_relativeY) ? itInput->m_mouse.m_relY : itInput->m_mouse.m_posY;
							m_messageBuffer[numMessages].setInputX((mouseAttributes.m_invertedX) ? -posX : posX);
							m_messageBuffer[numMessages].setInputY((mouseAttributes.m_invertedY) ? -posY : posY);
							m_messageBuffer[numMessages].setEntityHandler(inputComponent->m_entity);
							numMessages++;
							handled = true;
						}
					}
					if(handled)
					{
						itInput = input.erase(itInput);
						if(itInput == input.end())
							return numMessages;
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

KeyboardEvent InputSystem::mapXmlKeyNode(rapidxml::xml_node<>* node)
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

MouseMapAttributes InputSystem::mapXmlMouseNode(rapidxml::xml_node<>* node)
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