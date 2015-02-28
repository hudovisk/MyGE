
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>

class Shader
{
public:
	Shader()
		: m_vertexFilePath(""), m_fragmentFilePath(""), m_program(0),
		  m_initialised(false) { }
	~Shader() { destroy(); }

	bool init(std::string vertexFilePath, std::string fragmentFilePath);
	bool destroy();

	unsigned int getProgram() { return m_program; }

	bool isInitialised() { return m_initialised; }

private:

	std::string m_vertexFilePath;
	std::string m_fragmentFilePath;

	unsigned int m_program;
	bool m_initialised;
};


#endif