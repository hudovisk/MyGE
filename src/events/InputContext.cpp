/*
 * InputContext.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: hudo
 */

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <ctype.h>

#include "events/InputContext.h"
#include "debug/MyAssert.h"
#include "debug/Logger.h"

InputContext::InputContext()
{

}

bool InputContext::init(std::string filename)
{
	using namespace rapidxml;

	m_keyMap.clear();
	m_mouseMap.clear();

	FILE* file = fopen(filename.c_str(), "rb");
	if(file != nullptr)
	{
		fseek(file, 0L, SEEK_END);
		unsigned int size = ftell(file);

		fseek(file, 0L, SEEK_SET);

		char* buffer = (char*) calloc(size,sizeof(char));
		if(!buffer)
		{
			LOG(ERROR, "File: "<<filename<<" too big");
			fclose(file);

			return false;
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

			ASSERT(false,"InputContext init file: "<<filename<<" error, possibly wrong file or bad XML. parser error: "<<e.what());
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
					  m_keyMap[keyEvent].push_back(messageId);
					}
					else if(childName == "mouse")
					{
					  MouseMapAttributes mouseAttributes = mapXmlMouseNode(childNode);
					  mouseAttributes.m_messageId = messageId;
					  m_mouseMap[mouseAttributes.m_mouseCode].push_back(mouseAttributes);
					}
					else
					{
					  ASSERT(false,"InputContext init file: "<<filename<<" error, node name: "<<name<<" not identified.");
					}
					childNode = childNode->next_sibling();
				}
			}
			else
			{
			  ASSERT(false,"InputContext init file: "<<filename<<" error, node name: "<<name<<" not identified.");
			}
			node = node->next_sibling();
		}

		free(buffer);
		fclose(file);

		return true;
	}

	LOG(ERROR, "Couldn't open file: "<<filename);

	return false;
}

KeyboardEvent InputContext::mapXmlKeyNode(rapidxml::xml_node<>* node)
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
InputContext::MouseMapAttributes InputContext::mapXmlMouseNode(rapidxml::xml_node<>* node)
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

std::vector<InputMessage> InputContext::parse(std::list<InputEvent>& input)
{
	std::vector<InputMessage> messages;
	for(auto itInput = input.begin(); itInput != input.end(); ++itInput)
	{
		switch(itInput->m_type)
		{
			case InputEventType::KEYBOARD :
			{
				auto itFind = m_keyMap.find(itInput->m_key);
				if(itFind != m_keyMap.end())
				{
					for(unsigned int i=0; i < itFind->second.size(); i++)
					{
					  InputMessage message;
					  message.id = itFind->second.at(i);
					  messages.push_back(message);
// 					  itInput = input.erase(itInput);
//					  if(itInput == input.end())
//					    return messages;
					}
				}
				// else
				// {
				// 	std::cout<<"wrong key id key: "<<itInput->m_key.m_code<<" pressed Event: "<<itInput->m_key.m_pressed<<std::endl;
				// }
				break;
			}
 			case InputEventType::MOUSE :
 			{
 				auto itFind = m_mouseMap.find(itInput->m_mouse.m_code);	
 				if(itFind != m_mouseMap.end())
 				{
//					bool handled = false;
					for(auto itAtributes = itFind->second.begin(); itAtributes != itFind->second.end(); itAtributes++)
					{
						MouseMapAttributes mouseAttributes = *itAtributes;
						if(itInput->m_mouse.m_pressed == mouseAttributes.m_pressed)
						{
							InputMessage message;
							message.id = mouseAttributes.m_messageId;
							int posX = (mouseAttributes.m_relativeX) ? itInput->m_mouse.m_relX : itInput->m_mouse.m_posX;
							int posY = (mouseAttributes.m_relativeY) ? itInput->m_mouse.m_relY : itInput->m_mouse.m_posY;
							message.pos[0] = (mouseAttributes.m_invertedX) ? -posX : posX;
							message.pos[1] = (mouseAttributes.m_invertedY) ? -posY : posY;
							messages.push_back(message);
//							handled = true;
						}
					}
//					if(handled)
//					  itInput = input.erase(itInput);
				}
				if(itInput->m_mouse.m_pressed)
				{
					std::cout<<"Mouse x: "<<itInput->m_mouse.m_posX<<" relX: "<<itInput->m_mouse.m_relX<<std::endl;
					std::cout<<"Mouse y: "<<itInput->m_mouse.m_posY<<" relY: "<<itInput->m_mouse.m_relY<<std::endl;	
				}
				
				break;
			}
			default:
			{			
				break;
			}
		}
	}
	return messages;
}



