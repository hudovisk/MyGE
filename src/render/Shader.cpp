

#include "render/Shader.h"
#include "debug/Logger.h"

bool Shader::init(std::string vertexFilePath, std::string fragmentFilePath)
{
	if(m_initialised)
		destroy();

	m_vertexFilePath = vertexFilePath;
	m_fragmentFilePath = fragmentFilePath;

	//Create shaders
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	//Load vertex shader code
	FILE *vertexFile = fopen(vertexFilePath.c_str(), "rb");
	if(!vertexFile)
	{
		LOG(ERROR, "Could not open vertex shader file: "<<vertexFilePath);
		return false;
	}

	fseek(vertexFile, 0L, SEEK_END);
	int vertexFileSize = ftell(vertexFile);
	fseek(vertexFile, 0L, SEEK_SET);

	char* vertexFileBuffer = (char*) calloc(vertexFileSize,sizeof(char));
	if(!vertexFileBuffer)
	{
		LOG(ERROR, "Could not allocate memory for vertex shader file: "<<vertexFilePath);
		return false;
	}

	fread(vertexFileBuffer, vertexFileSize, 1, vertexFile);

	//Load fragment shader code
	FILE *fragmentFile = fopen(fragmentFilePath.c_str(), "rb");
	if(!fragmentFile)
	{
		LOG(ERROR, "Could not open fragment shader file: "<<fragmentFilePath);
		return false;
	}

	fseek(fragmentFile, 0L, SEEK_END);
	int fragmentFileSize = ftell(fragmentFile);
	fseek(fragmentFile, 0L, SEEK_SET);

	char* fragmentFileBuffer = (char*) calloc(fragmentFileSize,sizeof(char));
	if(!fragmentFileBuffer)
	{
		LOG(ERROR, "Could not allocate memory for fragment shader file: "<<fragmentFilePath);
		return false;
	}

	fread(fragmentFileBuffer, fragmentFileSize, 1, fragmentFile);

	int result = GL_FALSE;

	//Compile vertex shader
	const char* vertexSourcePointer = vertexFileBuffer;
	glShaderSource(vertexShaderId, 1, &vertexSourcePointer, &vertexFileSize);
	glCompileShader(vertexShaderId);
	glGetShaderiv(vertexShaderId,GL_COMPILE_STATUS, &result);
	if(GL_FALSE == result)
	{
		int infoLogLenght = 0;
		glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLenght);
		char* log = new char[infoLogLenght];
		glGetShaderInfoLog(vertexShaderId, infoLogLenght, NULL, log);
		LOG(ERROR,"Vertex shader compile error, "<<vertexFilePath<<" : "<<log);
		return false;
	}

	//Compile fragment shader
	char const * fragmentSourcePointer = fragmentFileBuffer;
	glShaderSource(fragmentShaderId, 1, &fragmentSourcePointer, &fragmentFileSize);
	glCompileShader(fragmentShaderId);
	glGetShaderiv(fragmentShaderId,GL_COMPILE_STATUS, &result);
	if(GL_FALSE == result)
	{
		int infoLogLenght = 0;
		glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLenght);
		char* log = new char[infoLogLenght];
		glGetShaderInfoLog(fragmentShaderId, infoLogLenght, NULL, log);
		LOG(ERROR,"Fragment shader compile error, "<<fragmentFilePath<<" : "<<log);
		return false;
	}

	//Linking the program
	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShaderId);
	glAttachShader(m_program, fragmentShaderId);
	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &result);
	if(GL_FALSE == result)
	{
		int infoLogLenght = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLenght);
		char* log = new char[infoLogLenght];
		glGetProgramInfoLog(m_program, infoLogLenght, NULL, log);
		LOG(ERROR, "Compile shader error: "<<log);
		return false;
	}

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	free(vertexFileBuffer);
	free(fragmentFileBuffer);

	fclose(vertexFile);
	fclose(fragmentFile);

	m_initialised = true;

	return true;
}

bool Shader::destroy()
{
	glDeleteProgram(m_program);

	return true;
}


