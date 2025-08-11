#ifndef SHADER_H
#define SHADER_H
#include <string_view>
#include <glm/glm.hpp>
class Shader {
 public:
  unsigned int id;
  void SetInt(const std::string_view name, int value) const;
  void SetFloat(const std::string_view name, float value) const;
  void SetVec2(const std::string_view name, const glm::vec2& value) const;
  void SetVec3(const std::string_view name, const glm::vec3& value) const;
  void SetMat4(const std::string_view name, const glm::mat4& value) const;
};
#endif
