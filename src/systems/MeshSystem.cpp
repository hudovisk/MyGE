
#include "systems/MeshSystem.h"

#include "core/Engine.h"
#include "debug/Logger.h"

#include "assimp-3.1.1/Importer.hpp"
#include "assimp-3.1.1/postprocess.h"
#include "assimp-3.1.1/scene.h"

void getVerticesAndIndices(std::vector<Vertex>& vertices, 
	std::vector<unsigned int>& indices, const aiMesh* aMesh);

MeshSystem::MeshSystem()
	: m_isInitialised(false)
{

}

MeshSystem::~MeshSystem()
{
	destroy();
}

bool MeshSystem::init()
{
	if(m_isInitialised)
		return false;

	if(!m_componentPool.init(100))
	{
		LOG(ERROR, "Couldnt initialise m_componentPool with 100 objects.");
		return false;
	}

	RenderStageEventData event;
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<MeshSystem,&MeshSystem::onUpdate>(this),
		event.getType());

	m_isInitialised = true;

	return true;
}

bool MeshSystem::destroy()
{
	if(m_isInitialised)
	{
		m_componentPool.destroy();
		m_isInitialised = false;
	}
	return true;
}

void MeshSystem::onUpdate(IEventDataPtr e)
{
	MeshComponent** meshes = m_componentPool.getUsedBufferCache();
	unsigned int numMeshes = m_componentPool.getUsedSize();

	for(unsigned int i=0; i<numMeshes; i++)
	{
		MeshComponent* mesh = meshes[i];

		m_getTransformMsg.setEntityHandler(mesh->m_entity);
		Engine::g_entityManager.sendMessage(&m_getTransformMsg);

		if(!m_getTransformMsg.isHandled())
		{
			LOG(ERROR, "Entity id: "<<mesh->m_entity<<" does not have a transform component.");
			continue;
		}
		Transform* transform = m_getTransformMsg.getTransform();

		Engine::g_renderManager.bindDefaultShader();
		Engine::g_renderManager.setMatrixUniform("modelViewProjection_Matrix", transform->getMatrix());

		for(unsigned int j=0; j<mesh->m_geometrics.size(); j++)
		{
			Engine::g_renderManager.render(mesh->m_geometrics[j]);
		}
	}
}

Component* MeshSystem::create()
{
	MeshComponent* mesh = m_componentPool.create();
	return mesh;
}

Component* MeshSystem::createFromJSON(const char* json)
{
	MeshComponent* mesh = m_componentPool.create();

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(json, 
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	
	if(!scene)
	{
		LOG(ERROR, "Error importing: "<<json<<". "<<importer.GetErrorString());
		return mesh;
	}

	for(unsigned int i=0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* aMesh = scene->mMeshes[i];
		
		std::vector<Vertex> vertices(aMesh->mNumVertices);
		std::vector<unsigned int> indices(aMesh->mNumFaces * 3);

		getVerticesAndIndices(vertices, indices, aMesh);

		mesh->m_geometrics.push_back(Engine::g_renderManager.createGeometric(vertices, indices));
	}

	return mesh;
}

void MeshSystem::release(Component* component)
{
	MeshComponent* mesh = dynamic_cast<MeshComponent*>(component);
	for(unsigned int i=0; i<mesh->m_geometrics.size(); i++)
	{
		Engine::g_renderManager.releaseGeometric(mesh->m_geometrics[i]);
	}

	m_componentPool.release(mesh);
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