
#ifndef GAME_OBJECT_FACTORY_H
#define GAME_OBJECT_FACTORY_H

#include "render/Geometric.h"

#include <vector>

class GameObjectFactory
{
public:
	GameObjectFactory();
	~GameObjectFactory();

	static void loadMesh(const char* filePath, std::vector<Geometric*>& mesh);
	
};

#endif //GAME_OBJECT_FACTORY_H