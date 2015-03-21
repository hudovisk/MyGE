#pragma once

#include "render/Shader.h"

class GeometricPassShader :	public Shader
{
public:
	GeometricPassShader();
	~GeometricPassShader();

	bool init(std::string vertexFilePath, std::string fragmentFilePath);

	void setWVPMatrix(const Matrix4& matrix);
	void setWorldMatrix(const Matrix4& matrix);

	void setDiffuseSampler(unsigned int unit);
	void setDiffuseColor(const Vec3& color);
	void setNormalSampler(unsigned int unit);

	void setDiffuseSamplerFlag(bool flag);
	void setNormalSamplerFlag(bool flag);

private:
	//Locations
	int m_locWVP;
	int m_locWorld;

	int m_locDiffuse;
	int m_locDiffuseColor;
	int m_locDiffuseFlag;

	int m_locNormal;
	int m_locNormalFlag;
};

