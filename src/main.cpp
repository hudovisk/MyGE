
#include "main.h"
#include "ResourceManager.h"

int main()
{
	Engine *engine = Engine::getInstance();

	IManager *resourceManager = new ResourceManager();
	engine->registerManager(resourceManager);

	if (engine->init())
		engine->start();
	else
		return -1;

	return 0;
}
