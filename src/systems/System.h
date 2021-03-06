
#ifndef SYSTEM_H
#define SYSTEM_H

#include "systems/Component.h"

#include "rapidjson/document.h"

class System
{
public:
	System() { }
	virtual ~System() { }
	
	virtual bool init() { return true; }
	virtual bool destroy() { return true; }

	virtual Component* create() = 0;
	virtual Component* createFromJSON(const rapidjson::Value& jsonObject) = 0;

	virtual void release(Component* component) = 0;
};

#endif