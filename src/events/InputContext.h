/*
 * InputContext.h
 *
 *  Created on: Mar 31, 2014
 *      Author: hudo
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <list>
 
#include "events/InputTypes.h"
#include "rapidxml-1.13/rapidxml.hpp"

class InputParsed
{
public:
	unsigned int id;
	union
	{
		float value;
		float pos[2];
	};
};

/**
 * @brief Maps input events into messages to be handled by the game objects
 * @details This class parses a xml file to map key bindings into especific game messages. It provides an easy and flexible
 * way of change key bindings.
 */
class InputContext
{
public:
	InputContext();

	bool init(std::string filename);

	std::vector<InputParsed> parse(std::list<InputEvent>& input);
private:

	class MouseMapAttributes
	{
	public:
		unsigned int m_mouseCode;
		unsigned int m_messageId;
		bool m_invertedX;
		bool m_relativeX;
		bool m_relativeY;
		bool m_invertedY;
		bool m_pressed;
	};

	using MouseMap = std::pair<unsigned int, std::vector<MouseMapAttributes>>;
	using KeyMap = std::pair<KeyboardEvent, std::vector<unsigned int>>;
	
	KeyboardEvent mapXmlKeyNode(rapidxml::xml_node<>* node);
	MouseMapAttributes mapXmlMouseNode(rapidxml::xml_node<>* node);
	
	void setInvertedAxis(MouseMapAttributes& mouseAttribute, rapidxml::xml_node<>* node);

	std::map<KeyboardEvent, std::vector<unsigned int>> m_keyMap;
	std::map<unsigned int, std::vector<MouseMapAttributes>> m_mouseMap;
	//joystick map
};

