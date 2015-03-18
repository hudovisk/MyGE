
#include "systems/MeshSystem.h"

#include "core/Engine.h"
#include "debug/Logger.h"

#include <iostream>

// void getVerticesAndIndices(std::vector<Vertex>& vertices, 
// 	std::vector<unsigned int>& indices, const aiMesh* aMesh);

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

	if(!m_shader.init("res/shaders/vertex.glsl", "res/shaders/fragment.glsl"))
	{
		LOG(ERROR, "Default MeshSystem shader could not initialise.");
		return false;
	}

	if(!m_shadowStencilShader.init("res/shaders/shadowStencilVertex.glsl", ""))
	{
		LOG(ERROR, "Default MeshSystem shadowStencil shader could not initialise.");
		return false;
	}

	GeometricPassEventData geometricEvent;
	ShadowStencilPassEventData shadowEvent;

	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<MeshSystem,&MeshSystem::geometricPass>(this),
		geometricEvent.getType());
	Engine::g_eventManager.addListenner(
		EventListenerDelegate::from_method<MeshSystem,&MeshSystem::shadowStencilPass>(this),
		shadowEvent.getType());

	m_isInitialised = true;

	return true;
}

bool MeshSystem::destroy()
{
	if(m_isInitialised)
	{
		LOG(INFO, "Destroying MeshSystem.");
		GeometricPassEventData geometricEvent;
		ShadowStencilPassEventData shadowEvent;

		Engine::g_eventManager.removeListenner(
			EventListenerDelegate::from_method<MeshSystem,&MeshSystem::geometricPass>(this),
			geometricEvent.getType());
		Engine::g_eventManager.removeListenner(
			EventListenerDelegate::from_method<MeshSystem,&MeshSystem::shadowStencilPass>(this),
			shadowEvent.getType());

		m_shader.destroy();
		m_shadowStencilShader.destroy();

		m_componentPool.destroy();
		
		m_isInitialised = false;
	}
	return true;
}

void MeshSystem::shadowStencilPass(IEventDataPtr e)
{
	ShadowStencilPassEventData* event = dynamic_cast<ShadowStencilPassEventData*>(e.get());

	MeshComponent** meshes = m_componentPool.getUsedBufferCache();
	unsigned int numMeshes = m_componentPool.getUsedSize();

	const Matrix4& invCamera = *(event->m_view);
	const Matrix4& projection = *(event->m_projection);
	if(event->m_stencil)
	{

	}
	else
		Engine::g_renderManager.bindShadowPass();

	Engine::g_renderManager.bindShader(m_shadowStencilShader);

	for(unsigned int i=0; i<numMeshes; i++)
	{
		MeshComponent* mesh = meshes[i];

		m_getTransformMsg.setHandled(false);
		m_getTransformMsg.setEntityHandler(mesh->m_entity);
		Engine::g_entityManager.sendMessage(&m_getTransformMsg);

		if(!m_getTransformMsg.isHandled())
		{
			LOG(ERROR, "Entity id: "<<mesh->m_entity<<
				" does not have a transform component.");
			continue;
		}
		Transform* transform = m_getTransformMsg.getTransform();
		
		const Matrix4 modelView = transform->getMatrix() * invCamera;

		m_shader.setMatrix4f("MVP_Matrix", modelView * projection);

		for(unsigned int j=0; j<mesh->m_geometrics.size(); j++)
		{
			Engine::g_renderManager.render(mesh->m_geometrics[j]);
		}
	}
}

void MeshSystem::geometricPass(IEventDataPtr e)
{
	Engine::g_renderManager.bindGeometricPass();

	MeshComponent** meshes = m_componentPool.getUsedBufferCache();
	unsigned int numMeshes = m_componentPool.getUsedSize();

	Engine::g_renderManager.bindShader(m_shader);

	const Matrix4& invCamera = 
		Engine::g_renderManager.getDefaultCameraTransform()->getInverseMatrix();
	const Matrix4& projection = Engine::g_renderManager.getDefaultCameraProjection();
	for(unsigned int i=0; i<numMeshes; i++)
	{
		MeshComponent* mesh = meshes[i];

		m_getTransformMsg.setHandled(false);
		m_getTransformMsg.setEntityHandler(mesh->m_entity);
		Engine::g_entityManager.sendMessage(&m_getTransformMsg);

		if(!m_getTransformMsg.isHandled())
		{
			LOG(ERROR, "Entity id: "<<mesh->m_entity<<
				" does not have a transform component.");
			continue;
		}
		Transform* transform = m_getTransformMsg.getTransform();
		
		const Matrix4 modelView = transform->getMatrix() * invCamera;

		m_shader.setMatrix4f("model_Matrix", transform->getMatrix());
		m_shader.setMatrix4f("modelView_Matrix", modelView);
		m_shader.setMatrix4f("MVP_Matrix", modelView * projection);

		for(unsigned int j=0; j<mesh->m_geometrics.size(); j++)
		{
			const Geometric* geometric = mesh->m_geometrics[j];

			int texCount = 0;
			if(geometric->m_material.m_diffuseTexture != nullptr)
			{
				Engine::g_renderManager.bind2DTexture(texCount, geometric->m_material.m_diffuseTexture);
				m_shader.set1i("gDiffuseSampler", texCount++);
				m_shader.set1i("useDiffSampler", 1);
			}
			else
			{
				m_shader.setVec3f("gDiffColor", geometric->m_material.m_diffColor);
				m_shader.set1i("useDiffSampler", 0);
			}

			// m_shader.set1f("gMaterial.shininess", geometric->m_material.m_shineness);

			if(geometric->m_material.m_normalsTexture != nullptr)
			{
				Engine::g_renderManager.bind2DTexture(texCount, geometric->m_material.m_normalsTexture);
				m_shader.set1i("gNormalsSampler", texCount++);
				m_shader.set1i("useNormalSampler", 1);
			}
			else
			{
				m_shader.set1i("useNormalSampler", 0);	
			}

			Engine::g_renderManager.render(mesh->m_geometrics[j]);
		}
	}
}

Component* MeshSystem::create()
{
	MeshComponent* mesh = m_componentPool.create();
	return mesh;
}

Component* MeshSystem::createFromJSON(const rapidjson::Value& jsonObject)
{
	MeshComponent* component = m_componentPool.create();

	for(auto itMember = jsonObject.MemberBegin();
		itMember != jsonObject.MemberEnd(); itMember++)
	{
		if(strcmp("file", itMember->name.GetString()) == 0)
		{
			Engine::g_resourceManager.loadOBJFile(itMember->value.GetString(), component->m_geometrics, ResourceManager::LOAD_EVERYTHING);
		}
	}

	return component;
}

// void MeshSystem::loadFile(MeshComponent* mesh, std::string filePath)
// {
// 	Assimp::Importer importer;
// 	const aiScene *scene = importer.ReadFile(filePath, 
// 		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
	
// 	//GET DIRECTORY TO LOAD TEXTURES CORRECTLY
// 	std::string::size_type slashIndex = filePath.find_last_of("/");
// 	std::string dir;
// 	if (slashIndex == std::string::npos) {
//       dir = ".";
// 	}
// 	else if (slashIndex == 0) {
// 		dir = "/";
// 	}
// 	else {
// 		dir = filePath.substr(0, slashIndex);
// 	}

// 	if(!scene)
// 	{
// 		LOG(ERROR, "Error importing: "<<filePath<<". "<<importer.GetErrorString());
// 		return;
// 	}

// 	//LOAD TEXTURES
// 	std::vector<Texture*> textures(scene->mNumMaterials);
// 	for(unsigned int i=0; i < scene->mNumMaterials; i++)
// 	{
// 		const aiMaterial* aMaterial = scene->mMaterials[i];
// 		if(aMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
// 		{
// 			aiString texturePath;
// 			if(aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath,
// 				NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
// 			{
// 				//CONVERT SLASHES FOR LINUX
// 				int aux = 0;
// 	  			while(texturePath.data[aux] != '\0')
// 	  			{
// 	  				if(texturePath.data[aux] == '\\')
// 	  					texturePath.data[aux] = '/';
// 	  				aux++;
// 	  			}
// 	  			std::string fullPath = dir + "/" + texturePath.data;
// 	  			// std::cout<<"Pos: "<<pos++<<" Texture: "<<fullPath<<std::endl;
//   				textures[i] = Engine::g_renderManager.createTextureFromImg(fullPath);
// 			}
// 		}
// 	}

// 	//LOAD VERTICES
// 	for(unsigned int i=0; i < scene->mNumMeshes; i++)
// 	{
// 		const aiMesh* aMesh = scene->mMeshes[i];
		
// 		std::vector<Vertex> vertices(aMesh->mNumVertices);
// 		std::vector<unsigned int> indices(aMesh->mNumFaces * 3);

// 		getVerticesAndIndices(vertices, indices, aMesh);

// 		Geometric* geometric = Engine::g_renderManager.createGeometric(vertices, indices);
// 		if(aMesh->HasTextureCoords(0))
// 		{
// 			// std::cout<<"Index: "<<aMesh->mMaterialIndex;
// 			// std::cout<<" Address: "<<textures[aMesh->mMaterialIndex]<<std::endl;
// 			geometric->m_texture = textures[aMesh->mMaterialIndex];
// 		}

// 		mesh->m_geometrics.push_back(geometric);
// 	}
// }

void MeshSystem::release(Component* component)
{
	MeshComponent* mesh = dynamic_cast<MeshComponent*>(component);
	for(unsigned int i=0; i<mesh->m_geometrics.size(); i++)
	{
		if(mesh->m_geometrics[i]->m_material.m_diffuseTexture)
		{
			Engine::g_renderManager.releaseTexture(mesh->m_geometrics[i]->m_material.m_diffuseTexture);	
		}
		if(mesh->m_geometrics[i]->m_material.m_normalsTexture)
		{
			Engine::g_renderManager.releaseTexture(mesh->m_geometrics[i]->m_material.m_normalsTexture);	
		}

		Engine::g_renderManager.releaseGeometric(mesh->m_geometrics[i]);
	}

	m_componentPool.release(mesh);
}

