
#ifndef LIGHT_SYSTEM_H
#define LIGHT_SYSTEM_H

#include "systems/System.h"

#include "events/Events.h"

#include "render/LightPassShader.h"
#include "render/Geometric.h"

#include "memory/ObjectPool.h"

#include "math/Vec3.h"
#include "math/Matrix4.h"

class LightComponent : public Component
{
public:
	LightComponent() { }
	~LightComponent() { }

private:
	friend class LightSystem;
	
	enum LIGHT_TYPE
	{
		POINT_TYPE = 0,
		DIRECTIONAL_TYPE,
		SPOT_TYPE,

		NUM_TYPES
	};

	int m_type;
	float m_ambIntensity;
	float m_difIntensity;
	float m_linCoefficient;
	float m_expCoefficient;
	float m_cteCoefficient;
	float m_radius; 
	float m_minSpotFactor;	
	Vec3 m_color;
	Vec3 m_direction; // directional light
};

class LightSystem : public System
{
public:
	LightSystem();
	~LightSystem();

	bool init();
	bool destroy();

	void render(IEventDataPtr e);

	Component* create();
	Component* createFromJSON(const rapidjson::Value& jsonObject);

	void release(Component* component);

private:
	bool initShaders();
	bool initGeometrics();

	void renderPointLights();
	void renderDirectionalLights();
	void renderSpotLights();

	int strToLightType(const char* str);

	bool m_isInitialised;

	LightPassShader m_lpSpotShader;
	LightPassShader m_lpPointShader;
	LightPassShader m_lpDirShader;

	Geometric* m_lightGeometrics[3];

	ObjectPool<LightComponent> m_componentPools[3];

	std::shared_ptr<ShadowStencilPassEventData> m_shadowStencilEvent;
};

#endif //LIGHT_SYSTEM_H