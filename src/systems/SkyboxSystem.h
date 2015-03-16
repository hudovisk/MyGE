
#ifndef SKYBOX_SYSTEM_H
#define SKYBOX_SYSTEM_H

#include "systems/System.h"

#include "render/Geometric.h"
#include "render/Shader.h"

#include "events/Events.h"

#include "memory/ObjectPool.h"

class SkyboxComponent : public Component
{
public:
	SkyboxComponent() { }
	~SkyboxComponent() { }
	
	unsigned int m_cubeMapTexture;
	float m_size;
};

class SkyboxSystem : public System
{
public:
	SkyboxSystem();
	~SkyboxSystem();

	bool init();
	bool destroy();

	void render(IEventDataPtr e);

	Component* create();
	Component* createFromJSON(const rapidjson::Value& jsonObject);

	virtual void release(Component* component);

private:
	bool m_initialised;

	ObjectPool<SkyboxComponent> m_componentPool;

	Geometric* m_boxGeometric;
	Matrix4 m_modelMatrix;
	Shader m_shader;
};

#endif //SKYBOX_SYSTEM_H