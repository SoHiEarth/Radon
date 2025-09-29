#include <classes/shader.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#define GET_LOCATION() glGetUniformLocation(id_, name)

void Shader::Use() const {
  glUseProgram(id_);
}

void Shader::SetInt(const char* name, int value) const {
  glUniform1i(GET_LOCATION(), value);
}

void Shader::SetFloat(const char* name, float value) const {
  glUniform1f(GET_LOCATION(), value);
}

void Shader::SetVec2(const char* name, const glm::vec2& value) const {
  glUniform2fv(GET_LOCATION(), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const char* name, const glm::vec3& value) const {
  glUniform3fv(GET_LOCATION(), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const char* name, const glm::mat4& value) const {
  glUniformMatrix4fv(GET_LOCATION(), 1, GL_FALSE, glm::value_ptr(value));
}
