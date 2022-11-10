#include "Shader.h"
#include <fstream>
#include <sstream>
#include "glm/gtc/type_ptr.hpp"

Shader::Shader(const char* vs_filepath, const char* fs_filepath)
{
	std::ifstream file(vs_filepath);
	std::stringstream ss[2];
	std::string line;
	while (std::getline(file, line))
		ss[0] << line << std::endl;
	file.close();
	file.open(fs_filepath);
	while (std::getline(file, line))
		ss[1] << line << std::endl;
	m_ShaderID = CreateShader(ss[0].str(), ss[1].str());
}
Shader::~Shader()
{
	glDeleteProgram(m_ShaderID);
}

void Shader::Bind() const
{
	glUseProgram(m_ShaderID);
}
void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetVec4(const std::string name, float v1, float v2, float v3, float v4)
{
	if (m_UniformCache.find(name) != m_UniformCache.end())
		glUniform4f(m_UniformCache[name], v1, v2, v3, v4);
	else {
		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1) {
			std::cout << "Get the invalid location -1!" << name << std::endl;
		}
		m_UniformCache[name] = location;
		glUniform4f(location, v1, v2, v3, v4);
	}
}
void Shader::SetVec3(const std::string name, float v1, float v2, float v3)
{
	if (m_UniformCache.find(name) != m_UniformCache.end())
		glUniform3f(m_UniformCache[name], v1, v2, v3);
	else {
		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1) {
			std::cout << "Get the invalid location -1!" << name << std::endl;
		}
		m_UniformCache[name] = location;
		glUniform3f(location, v1, v2, v3);
	}
}

void Shader::SetVec3(const std::string name, glm::vec3 vector)
{
	if (m_UniformCache.find(name) != m_UniformCache.end())
		glUniform3f(m_UniformCache[name], vector.x, vector.y, vector.z);
	else {
		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1) {
			std::cout << "Get the invalid location -1!" << name << std::endl;
		}
		m_UniformCache[name] = location;
		glUniform3f(location, vector.x, vector.y, vector.z);
	}
}

void Shader::SetFloat(const std::string name, float v)
{
	if (m_UniformCache.find(name) != m_UniformCache.end())
		glUniform1f(m_UniformCache[name], v);
	else {
		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1) {
			std::cout << "Get the invalid location -1!" << name << std::endl;
		}
		m_UniformCache[name] = location;
		glUniform1f(location, v);
	}
}

void Shader::SetUniformMatrx(const std::string name, glm::mat4 matrix)
{
	if (m_UniformCache.find(name) != m_UniformCache.end())
		glUniformMatrix4fv(m_UniformCache[name], 1, GL_FALSE, glm::value_ptr(matrix));
	else {
		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1) {
			std::cout << "Get the invalid location -1!" << name << std::endl;
		}
		m_UniformCache[name] = location;
		glUniformMatrix4fv(m_UniformCache[name], 1, GL_FALSE, glm::value_ptr(matrix));
	}
}

void Shader::SetUniformMatrx(const std::string name, glm::mat3 matrix)
{
	if (m_UniformCache.find(name) != m_UniformCache.end())
		glUniformMatrix4fv(m_UniformCache[name], 1, GL_FALSE, glm::value_ptr(matrix));
	else {
		int location = glGetUniformLocation(m_ShaderID, name.c_str());
		if (location == -1) {
			std::cout << "Get the invalid location -1!" << name << std::endl;
		}
		m_UniformCache[name] = location;
		glUniformMatrix4fv(m_UniformCache[name], 1, GL_FALSE, glm::value_ptr(matrix));
	}
}

unsigned int Shader::CompileShader(GLenum type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int flag;
	glGetShaderiv(id, GL_COMPILE_STATUS, &flag);
	if (!flag) {
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vetex" : "fragment") <<
			"shader !" << std::endl;
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;
	}

	return id;
}
unsigned int Shader::CreateShader(const std::string& VertexShader, const std::string& FragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, VertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	int flag;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &flag);
	if (!flag) {
		std::cout << "Failed to validate shader!" << std::endl;
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)malloc(length * sizeof(char));
		glGetProgramInfoLog(program, length, &length, message);
		std::cout << message << std::endl;

		glDeleteShader(vs);
		glDeleteShader(fs);
		glDeleteProgram(program);
		return 0;
	}
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}