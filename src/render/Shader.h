
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>

class Shader
{
public:
	Shader()
		: m_vertexFilePath(NULL), m_fragmentFilePath(NULL), m_program(0),
		  m_initialised(false) { }
	~Shader() { destroy(); }

	bool init(char* vertexFilePath, char* fragmentFilePath);
	bool destroy();

	unsigned int getProgram() { return m_program; }

	bool isInitialised() { return m_initialised; }

private:

	char* m_vertexFilePath;
	char* m_fragmentFilePath;

	unsigned int m_program;
	bool m_initialised;
};


#endif