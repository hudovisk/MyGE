

#include "render/Shader.h"
#include "debug/Logger.h"

bool Shader::init(std::string vertexFilePath, std::string fragmentFilePath)
{
	if(m_initialised)
		destroy();

	m_vertexFilePath = vertexFilePath;
	m_fragmentFilePath = fragmentFilePath;

	//Create shaders
	GLuint vertexShaderId = 0; 
	GLuint fragmentShaderId = 0;
	if(!vertexFilePath.empty())
	{
		vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		if(!compileShader(vertexFilePath, vertexShaderId))
			return false;
	}
	if(!fragmentFilePath.empty())
	{
		fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);	
		if(!compileShader(fragmentFilePath, fragmentShaderId))
			return false;
	} 

	//Linking the program
	m_program = glCreateProgram();
	if(vertexShaderId > 0)
	{
		glAttachShader(m_program, vertexShaderId);
	}
	if(fragmentShaderId > 0)
	{
		glAttachShader(m_program, fragmentShaderId);	
	}

	glLinkProgram(m_program);

	int result = GL_FALSE;
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

	m_initialised = true;

	return true;
}

bool Shader::compileShader(std::string shaderPath, unsigned int shaderId)
{
	//Load shader code
	FILE *file = fopen(shaderPath.c_str(), "rb");
	if(!file)
	{
		LOG(ERROR, "Could not open shader file: "<<shaderPath);
		return false;
	}

	fseek(file, 0L, SEEK_END);
	int fileSize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	char* fileBuffer = (char*) calloc(fileSize,sizeof(char));
	if(!fileBuffer)
	{
		LOG(ERROR, "Could not allocate memory for shader file: "<<fileBuffer);
		fclose(file);
		return false;
	}

	fread(fileBuffer, fileSize, 1, file);

	int result = GL_FALSE;

	//Compile shader
	const char* sourcePointer = fileBuffer;
	glShaderSource(shaderId, 1, &sourcePointer, &fileSize);
	glCompileShader(shaderId);
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	if(GL_FALSE == result)
	{
		int infoLogLenght = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLenght);
		char* log = new char[infoLogLenght];
		glGetShaderInfoLog(shaderId, infoLogLenght, NULL, log);
		LOG(ERROR,"Shader compile error, "<<shaderPath<<" : "<<log);

		delete [] log;
		free(fileBuffer);
		fclose(file);

		return false;
	}

	free(fileBuffer);
	fclose(file);

	return true;
}

void Shader::setMatrix4f(std::string uniformName, const Matrix4& value)
{
	int uniformId = glGetUniformLocation(m_program, uniformName.c_str());
	glUniformMatrix4fv(uniformId, 1, true, value.m_data);
}

void Shader::setVec3f(std::string uniformName, const Vec3& value)
{
	int uniformId = glGetUniformLocation(m_program, uniformName.c_str());
	glUniform3f(uniformId, value.m_data[0], value.m_data[1], value.m_data[2]);
}

void Shader::set1i(std::string uniformName, const int value)
{
	int uniformId = glGetUniformLocation(m_program, uniformName.c_str());
	glUniform1i(uniformId, value);	
}

void Shader::set1f(std::string uniformName, const float value)
{
	int uniformId = glGetUniformLocation(m_program, uniformName.c_str());
	glUniform1f(uniformId, value);	
}

void Shader::set2f(std::string uniformName, const float value, const float value2)
{
	int uniformId = glGetUniformLocation(m_program, uniformName.c_str());
	glUniform2f(uniformId, value, value2);	
}

bool Shader::destroy()
{
	if(m_initialised)
	{
		glDeleteProgram(m_program);	
		m_initialised = false;
	}

	return true;
}


