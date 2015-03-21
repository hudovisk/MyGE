#pragma once

#include "render/Shader.h"

class LightPassShader : public Shader
{
public:
	LightPassShader();
	~LightPassShader();

	bool init(std::string vertexFilePath, std::string fragmentFilePath);

	void setLightPos(const Vec3& position);
	void setLightDir(const Vec3& direction);
	void setLightColor(const Vec3& color);
	void setMinSpotFactor(float factor);
	void setAmbIntensity(float intensity);
	void setDiffIntensity(float intensity);

	void setExpoAttenuation(float coefficient);

	void setScreenSize(float width, float height);
	void setEyePos(const Vec3& pos);

	void setWVPLight(const Matrix4& matrix);
	void setWVP(const Matrix4& matrix);

	void setPositionSampler(unsigned int unit);
	void setColorSampler(unsigned int unit);
	void setNormalSampler(unsigned int unit);
	void setShadowSampler(unsigned int unit);

private:

	int m_locWVP;

	struct
	{
		int pos;
		int dir;
		int minSpotFactor;
		int color;
		int ambIntensity;
		int diffIntensity;
	} m_locLight;

	int m_locAttExpo;

	int m_locScreenSize;
	int m_locEyePos;
	int m_locWVPLight;

	int m_locPositionSampler;
	int m_locColorSampler;
	int m_locNormalSampler;
	int m_locShadowSampler;
};