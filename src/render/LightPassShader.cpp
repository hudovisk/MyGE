
#include "render/LightPassShader.h"

LightPassShader::LightPassShader()
{
}

LightPassShader::~LightPassShader()
{
	destroy();
}

bool LightPassShader::init(std::string vertexFilePath, std::string fragmentFilePath) {
	if (!Shader::init(vertexFilePath, fragmentFilePath))
		return false;

	m_locWVP = glGetUniformLocation(m_program, "WVP_Matrix");

	m_locLight.pos = glGetUniformLocation(m_program, "gLight.pos");
	m_locLight.dir = glGetUniformLocation(m_program, "gLight.direction");
	m_locLight.color = glGetUniformLocation(m_program, "gLight.color");
	m_locLight.minSpotFactor = glGetUniformLocation(m_program, "gLight.minSpotFactor");
	m_locLight.ambIntensity = glGetUniformLocation(m_program, "gLight.ambIntensity");
	m_locLight.diffIntensity = glGetUniformLocation(m_program, "gLight.diffIntensity");

	m_locAttExpo = glGetUniformLocation(m_program, "gLight.att.expo");

	m_locScreenSize = glGetUniformLocation(m_program, "gScreenSize");
	m_locEyePos = glGetUniformLocation(m_program, "gEyeWorldPos");
	m_locWVPLight = glGetUniformLocation(m_program, "WVPLight_Matrix");

	m_locPositionSampler = glGetUniformLocation(m_program, "gPositionMap");
	m_locColorSampler = glGetUniformLocation(m_program, "gColorMap");
	m_locNormalSampler = glGetUniformLocation(m_program, "gNormalMap");
	m_locShadowSampler = glGetUniformLocation(m_program, "gShadowMap");

	return true;
}


void LightPassShader::setLightPos(const Vec3& position) {
	glUniform3f(m_locLight.pos, position.m_data[0], position.m_data[1], position.m_data[2]);
}

void LightPassShader::setLightDir(const Vec3& direction) {
	glUniform3f(m_locLight.dir , direction.m_data[0], direction.m_data[1], direction.m_data[2]);
}

void LightPassShader::setLightColor(const Vec3& color) {
	glUniform3f(m_locLight.color, color.m_data[0], color.m_data[1], color.m_data[2]);
}

void LightPassShader::setMinSpotFactor(float factor) {
	glUniform1f(m_locLight.minSpotFactor, factor);
}

void LightPassShader::setAmbIntensity(float intensity) {
	glUniform1f(m_locLight.ambIntensity, intensity);
}

void LightPassShader::setDiffIntensity(float intensity) {
	glUniform1f(m_locLight.diffIntensity, intensity);
}

void LightPassShader::setExpoAttenuation(float coefficient) {
	glUniform1f(m_locAttExpo, coefficient);
}

void LightPassShader::setScreenSize(float width, float height) {
	glUniform2f(m_locScreenSize, width, height);
}

void LightPassShader::setEyePos(const Vec3& position) {
	glUniform3f(m_locEyePos, position.m_data[0], position.m_data[1], position.m_data[2]);
}

void LightPassShader::setWVPLight(const Matrix4& matrix) {
	glUniformMatrix4fv(m_locWVPLight, 1, true, matrix.m_data);
}

void LightPassShader::setWVP(const Matrix4& matrix) {
	glUniformMatrix4fv(m_locWVP, 1, true, matrix.m_data);
}

void LightPassShader::setPositionSampler(unsigned int unit) {
	glUniform1i(m_locPositionSampler, unit);
}

void LightPassShader::setColorSampler(unsigned int unit) {
	glUniform1i(m_locColorSampler, unit);
}

void LightPassShader::setNormalSampler(unsigned int unit) {
	glUniform1i(m_locNormalSampler, unit);
}

void LightPassShader::setShadowSampler(unsigned int unit) {
	glUniform1i(m_locShadowSampler, unit);
}
