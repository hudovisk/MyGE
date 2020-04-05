
#include "ResourceManager.h"
#include "Engine.h"

#include "debug/Logger.h"

ResourceManager::ResourceManager()
		: m_initialised(false)
{
}

ResourceManager::~ResourceManager()
{
	destroy();
}

bool ResourceManager::init()
{
	m_initialised = true;

	return true;
}

bool ResourceManager::destroy()
{
	if (m_initialised)
	{
		m_initialised = false;
	}

	return true;
}

// void ResourceManager::loadOBJFile(std::string filePath,
// 																	std::vector<Geometric *> &geometrics, unsigned int flags)
// {
// 	int aFlags = 0;
// 	aFlags |= aiProcess_Triangulate; // for position (always needed)
// 	if (flags & LOAD_NORMALS)
// 		aFlags |= aiProcess_GenSmoothNormals;
// 	if (flags & LOAD_TEXTURES)
// 		aFlags |= aiProcess_FlipUVs;
// 	if (flags & LOAD_TANGENTS)
// 		aFlags |= aiProcess_CalcTangentSpace;

// 	Assimp::Importer importer;
// 	const aiScene *scene = importer.ReadFile(filePath, aFlags);

// 	//GET DIRECTORY TO LOAD TEXTURES CORRECTLY
// 	std::string::size_type slashIndex = filePath.find_last_of("/");
// 	std::string dir;
// 	if (slashIndex == std::string::npos)
// 	{
// 		dir = ".";
// 	}
// 	else if (slashIndex == 0)
// 	{
// 		dir = "/";
// 	}
// 	else
// 	{
// 		dir = filePath.substr(0, slashIndex);
// 	}

// 	if (!scene)
// 	{
// 		LOG(ERROR, "Error importing: " << filePath << ". " << importer.GetErrorString());
// 		return;
// 	}

// 	//LOAD MATERIALS
// 	m_tempTexturesMap.clear();
// 	std::vector<Material> materials(scene->mNumMaterials);
// 	loadMaterials(materials, scene, dir);

// 	//LOAD VERTICES
// 	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
// 	{
// 		const aiMesh *aMesh = scene->mMeshes[i];

// 		Geometric *geometric = loadGeometric(aMesh, flags);

// 		geometric->m_material = materials[aMesh->mMaterialIndex];

// 		geometrics.push_back(geometric);
// 	}
// }

// Texture *ResourceManager::getTexture(aiTextureType textureType,
// 																		 const aiMaterial *aMaterial, const std::string &dir)
// {
// 	if (aMaterial->GetTextureCount(textureType) > 0)
// 	{
// 		aiString texturePath;
// 		if (aMaterial->GetTexture(textureType, 0, &texturePath,
// 															NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
// 		{
// 			//CONVERT SLASHES FOR LINUX
// 			int aux = 0;
// 			while (texturePath.data[aux] != '\0')
// 			{
// 				if (texturePath.data[aux] == '\\')
// 					texturePath.data[aux] = '/';
// 				aux++;
// 			}
// 			std::string fullPath = dir + "/" + texturePath.data;
// 			auto itFind = m_tempTexturesMap.find(fullPath);
// 			if (itFind != m_tempTexturesMap.end())
// 			{
// 				return itFind->second;
// 			}
// 			else
// 			{
// 				Texture *texture = Engine::g_renderManager.createTextureFromImg(fullPath);
// 				m_tempTexturesMap[fullPath] = texture;
// 				return texture;
// 			}
// 		}
// 	}
// 	return nullptr;
// }

// void ResourceManager::loadMaterials(std::vector<Material> &materials,
// 																		const aiScene *scene, const std::string &dir)
// {
// 	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
// 	{
// 		const aiMaterial *aMaterial = scene->mMaterials[i];
// 		aiString name;
// 		aMaterial->Get(AI_MATKEY_NAME, name);

// 		materials[i].m_diffuseTexture = getTexture(aiTextureType_DIFFUSE, aMaterial, dir);
// 		materials[i].m_normalsTexture = getTexture(aiTextureType_HEIGHT, aMaterial, dir);

// 		aiColor3D color(0.f, 0.f, 0.f);
// 		if (AI_SUCCESS != aMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color))
// 		{
// 			LOG(WARN, "No ambient color for material: " << name.data << " in dir: " << dir);
// 		}
// 		else
// 		{
// 			materials[i].m_ambColor = Vec3(color.r, color.g, color.b);
// 		}

// 		if (AI_SUCCESS != aMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color))
// 		{
// 			LOG(WARN, "No diffuse color for material: " << name.data << " in dir: " << dir);
// 		}
// 		else
// 		{
// 			materials[i].m_diffColor = Vec3(color.r, color.g, color.b);
// 		}

// 		if (AI_SUCCESS != aMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color))
// 		{
// 			LOG(WARN, "No ambient color for material: " << name.data << " in dir: " << dir);
// 		}
// 		else
// 		{
// 			materials[i].m_specColor = Vec3(color.r, color.g, color.b);
// 		}

// 		if (AI_SUCCESS != aMaterial->Get(AI_MATKEY_SHININESS, (materials[i].m_shininess)))
// 		{
// 			LOG(WARN, "No shininess for material: " << name.data << " in dir: " << dir);
// 			materials[i].m_shininess = 1;
// 		}
// 	}
// }

// Geometric *ResourceManager::loadGeometric(const aiMesh *aMesh, int flags)
// {
// 	unsigned int dataSize = 3;
// 	if (flags & LOAD_NORMALS)
// 		dataSize += 3;
// 	if (flags & LOAD_TEXTURES)
// 		dataSize += 2;
// 	if (flags & LOAD_TANGENTS)
// 		dataSize += 3;

// 	const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

// 	float *verticesData = new float[dataSize * aMesh->mNumVertices];
// 	unsigned int *indices = new unsigned int[aMesh->mNumFaces * 3];

// 	for (unsigned int i = 0; i < aMesh->mNumVertices; i++)
// 	{
// 		const aiVector3D *aPos = &(aMesh->mVertices[i]);
// 		const aiVector3D *aNormal = &(aMesh->mNormals[i]);
// 		const aiVector3D *aTangent = aMesh->HasTextureCoords(0) ? &(aMesh->mTangents[i]) : &zero3D;
// 		const aiVector3D *aTexCoord = aMesh->HasTextureCoords(0) ? &(aMesh->mTextureCoords[0][i]) : &zero3D;

// 		unsigned int offset = 0;

// 		verticesData[offset + i * dataSize] = aPos->x;
// 		offset++;
// 		verticesData[offset + i * dataSize] = aPos->y;
// 		offset++;
// 		verticesData[offset + i * dataSize] = aPos->z;
// 		offset++;

// 		if (flags & LOAD_NORMALS)
// 		{
// 			verticesData[offset + i * dataSize] = aNormal->x;
// 			offset++;
// 			verticesData[offset + i * dataSize] = aNormal->y;
// 			offset++;
// 			verticesData[offset + i * dataSize] = aNormal->z;
// 			offset++;
// 		}
// 		if (flags & LOAD_TEXTURES)
// 		{
// 			verticesData[offset + i * dataSize] = aTexCoord->x;
// 			offset++;
// 			verticesData[offset + i * dataSize] = aTexCoord->y;
// 			offset++;
// 		}
// 		if (flags & LOAD_TANGENTS)
// 		{
// 			verticesData[offset + i * dataSize] = aTangent->x;
// 			offset++;
// 			verticesData[offset + i * dataSize] = aTangent->y;
// 			offset++;
// 			verticesData[offset + i * dataSize] = aTangent->z;
// 			offset++;
// 		}
// 	}

// 	for (unsigned int t = 0; t < aMesh->mNumFaces; t++)
// 	{
// 		const aiFace *face = &aMesh->mFaces[t];

// 		indices[t * 3] = face->mIndices[0];
// 		indices[t * 3 + 1] = face->mIndices[1];
// 		indices[t * 3 + 2] = face->mIndices[2];
// 	}

// 	Geometric *geometric = Engine::g_renderManager.createGeometric(verticesData, aMesh->mNumVertices,
// 																																 indices, aMesh->mNumFaces * 3, flags);

// 	delete[] verticesData;
// 	delete[] indices;

// 	return geometric;
// }
