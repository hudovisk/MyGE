
#include "systems/System.h"

#include "events/Events.h"

#include "render/Shader.h"
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
	Vec3 m_color;
	Vec3 m_direction;
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
	void renderDirectionalLights(LightComponent* component);
	void renderSpotLights(LightComponent* component);

	void computeRadius(LightComponent* component);

	int strToLightType(const char* str);

	bool m_isInitialised;

	Shader m_lightShaders[3];

	Geometric* m_lightGeometrics[3];

	ObjectPool<LightComponent> m_componentPools[3];
};