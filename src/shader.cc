#include <classes/shader.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

void Shader::Use() {
  glUseProgram(id);
}

void Shader::SetInt(const std::string_view name, int value) const {
  glUniform1i(
      glGetUniformLocation(id, name.data()),
      value
  );
}

void Shader::SetFloat(const std::string_view name, float value) const {
  glUniform1f(
      glGetUniformLocation(id, name.data()),
      value
  );
}

void Shader::SetVec2(const std::string_view name, const glm::vec2& value) const {
  glUniform2fv(
      glGetUniformLocation(id, name.data()),
      1,
      glm::value_ptr(value)
  );
}

void Shader::SetVec3(const std::string_view name, const glm::vec3& value) const {
  glUniform3fv(
      glGetUniformLocation(id, name.data()),
      1,
      glm::value_ptr(value)
  );
}

void Shader::SetMat4(const std::string_view name, const glm::mat4& value) const {
  glUniformMatrix4fv(
      glGetUniformLocation(id, name.data()),
      1,
      GL_FALSE,
      glm::value_ptr(value)
  );
}
