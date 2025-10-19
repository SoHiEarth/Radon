#include <classes/shader.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#define GET_LOCATION() glGetUniformLocation(id_, name.c_str())

void Shader::Use() const {
  glUseProgram(id_);
}

void Shader::SetInt(const std::string& name, int value) const {
  glUniform1i(GET_LOCATION(), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
  glUniform1f(GET_LOCATION(), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) const {
  glUniform2fv(GET_LOCATION(), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
  glUniform3fv(GET_LOCATION(), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const {
  glUniformMatrix4fv(GET_LOCATION(), 1, GL_FALSE, glm::value_ptr(value));
}
