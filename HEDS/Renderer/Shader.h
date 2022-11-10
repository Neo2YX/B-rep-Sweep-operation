#pragma once
#include <glad/glad.h>
#include <iostream>
#include <unordered_map>
#include "glm/glm.hpp"

class Shader
{
private:
	unsigned int m_ShaderID;
	std::unordered_map<std::string, int> m_UniformCache;
public:
	Shader(const char* vs_filepath, const char* fs_filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	unsigned int GetID() const { return m_ShaderID; }

	//setUniform
	void SetVec4(const std::string name, float v1, float v2, float v3, float v4);
	void SetVec3(const std::string name, float v1, float v2, float v3);
	void SetVec3(const std::string name, glm::vec3 vector);
	void SetFloat(const std::string name, float v);
	void SetUniformMatrx(const std::string name, glm::mat4 matrix);
	void SetUniformMatrx(const std::string name, glm::mat3 matrix);

private:
	unsigned int CompileShader(GLenum type, const std::string& source);
	unsigned int CreateShader(const std::string& VertexShader, const std::string& FragmentShader);
};
