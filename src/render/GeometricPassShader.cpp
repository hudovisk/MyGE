#include "render/GeometricPassShader.h"


GeometricPassShader::GeometricPassShader()
	: m_locWVP(-1), m_locWorld(-1), m_locDiffuse(-1),
	m_locDiffuseFlag(-1), m_locNormal(-1), m_locNormalFlag(-1)
{
}


GeometricPassShader::~GeometricPassShader()
{
	destroy();
}

bool GeometricPassShader::init(std::string vertexFilePath, std::string fragmentFilePath)
{
	if (!Shader::init(vertexFilePath, fragmentFilePath))
		return false;

	m_locWVP = glGetUniformLocation(m_program, "WVP_Matrix");
	m_locWorld = glGetUniformLocation(m_program, "World_Matrix");
	m_locDiffuse = glGetUniformLocation(m_program, "gDiffuseSampler");
	m_locDiffuseColor = glGetUniformLocation(m_program, "gDiffColor");
	m_locDiffuseFlag = glGetUniformLocation(m_program, "useDiffSampler");
	m_locNormal = glGetUniformLocation(m_program, "gNormalsSampler");
	m_locNormalFlag = glGetUniformLocation(m_program, "useNormalSampler");

	return true;
}

void GeometricPassShader::setWVPMatrix(const Matrix4& matrix)
{
	glUniformMatrix4fv(m_locWVP, 1, true, matrix.m_data);
}

void GeometricPassShader::setWorldMatrix(const Matrix4& matrix)
{
	glUniformMatrix4fv(m_locWorld, 1, true, matrix.m_data);
}

void GeometricPassShader::setDiffuseSampler(unsigned int unit)
{
	glUniform1i(m_locDiffuse, unit);
}

void GeometricPassShader::setDiffuseColor(const Vec3& color)
{
	glUniform3f(m_program, color.m_data[0], color.m_data[1], color.m_data[2]);
}

void GeometricPassShader::setNormalSampler(unsigned int unit)
{
	glUniform1i(m_locNormal, unit);
}

void GeometricPassShader::setDiffuseSamplerFlag(bool flag)
{
	glUniform1i(m_locDiffuseFlag, flag);
}

void GeometricPassShader::setNormalSamplerFlag(bool flag)
{
	glUniform1i(m_locNormalFlag, flag);
}


