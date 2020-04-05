
#pragma once

#include "IManager.h"

// #include "render/Geometric.h"

// #include "assimp/Importer.hpp"
// #include "assimp/postprocess.h"
// #include "assimp/scene.h"

#include <string>
#include <vector>
#include <map>

class ResourceManager : public IManager
{
public:
	ResourceManager();
	~ResourceManager() override;

	IManagerHandler getHandler() const override { return IManagerHandler::RESOURCE_MANAGER; }

	bool init() override;
	bool destroy() override;

	// void loadOBJFile(std::string filePath, std::vector<Geometric *> &geometrics, unsigned int flags);

	// enum
	// {
	// 	LOAD_POSITIONS = 0x00, //always needed
	// 	LOAD_NORMALS = 0x02,
	// 	LOAD_TEXTURES = 0x04,
	// 	LOAD_TANGENTS = 0x0C, // need textures
	// 	LOAD_EVERYTHING = 0xFF
	// };

private:
	// Texture *getTexture(aiTextureType textureType, const aiMaterial *aMaterial,
	// 										const std::string &dir);
	// void loadMaterials(std::vector<Material> &materials,
	// 									 const aiScene *scene, const std::string &dir);
	// Geometric *loadGeometric(const aiMesh *aMesh, int flags);

	bool m_initialised;

	// std::map<std::string, Texture *> m_tempTexturesMap;
};
