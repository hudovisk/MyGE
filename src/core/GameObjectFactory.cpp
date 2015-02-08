
#include "core/GameObjectFactory.h"

#include "core/Engine.h"

#include "assimp-3.1.1/Importer.hpp"
#include "assimp-3.1.1/postprocess.h"
#include "assimp-3.1.1/scene.h"

void getVerticesAndIndices(std::vector<Vertex>& vertices, 
	std::vector<unsigned int>& indices, const aiMesh* aMesh);

void GameObjectFactory::loadMesh(const char* filePath, std::vector<Geometric*>& mesh)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filePath, 
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	
	if(!scene)
	{
		LOG(ERROR, "Error importing: "<<filePath<<". "<<importer.GetErrorString());
		return;
	}

	for(unsigned int i=0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* aMesh = scene->mMeshes[i];
		
		std::vector<Vertex> vertices(aMesh->mNumVertices);
		std::vector<unsigned int> indices(aMesh->mNumFaces * 3);

		getVerticesAndIndices(vertices, indices, aMesh);

		mesh.push_back(Engine::g_renderManager.createGeometric(vertices, indices));
	}
}

void getVerticesAndIndices(std::vector<Vertex>& vertices, 
	std::vector<unsigned int>& indices, const aiMesh* aMesh)
{
	const aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	
	for(unsigned int i=0; i < aMesh->mNumVertices; i++)
	{
		const aiVector3D* aPos      = &(aMesh->mVertices[i]);
		const aiVector3D* aNormal   = &(aMesh->mNormals[i]);
		const aiVector3D* aTexCoord = aMesh->HasTextureCoords(0) ?
			&(aMesh->mTextureCoords[0][i]) : &zero3D;

		Vertex v(Vec3(aPos->x, aPos->y, aPos->z),
					Vec3(aNormal->x, aNormal->y, aNormal->z),
					Vec3(aTexCoord->x, aTexCoord->y, 0));

		vertices[i] = v;
	}

	for(unsigned int t = 0; t < aMesh->mNumFaces; t++) 
	{
		const aiFace *face = &aMesh->mFaces[t];

		indices[t*3] 			= face->mIndices[0];
		indices[t*3 + 1] 	= face->mIndices[1];
		indices[t*3 + 2] 	= face->mIndices[2];
	}
}