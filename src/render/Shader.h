
#ifndef SHADER_H
#define SHADER_H

#include "math/Matrix4.h"
#include "math/Vec3.h"

#include <GL/glew.h>
#include <string>

class Shader
{
public:

	Shader()
		: m_vertexFilePath(""), m_fragmentFilePath(""), m_program(0),
		  m_initialised(false) { }
	virtual ~Shader() { destroy(); }

	virtual bool init(std::string vertexFilePath, std::string fragmentFilePath);
	bool destroy();

	void setMatrix4f(std::string uniformName, const Matrix4& value);
	void set1i(std::string uniformNamem, const int value);
	void set1f(std::string uniformName, const float value);
	void set2f(std::string uniformName, const float value, const float value2);
	void setVec3f(std::string uniformName, const Vec3& value);

	unsigned int getProgram() { return m_program; }

	bool isInitialised() { return m_initialised; }

protected:
	friend class RenderManager;

	bool compileShader(std::string shaderPath, unsigned int shaderId);

	std::string m_vertexFilePath;
	std::string m_fragmentFilePath;

	unsigned int m_program;
	bool m_initialised;
};


#endif