
#include "main.h"

int main(int argc, char** argv)
{
	Engine engine;

	if(engine.init())
		engine.start();
	else
		return -1;

	return 0;
}
