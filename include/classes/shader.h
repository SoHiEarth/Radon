#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>
#include <utility>

class Shader {
public:
  const std::string kVertPath, kFragPath;
  Shader(std::string vert, std::string frag)
      : kVertPath(std::move(vert)), kFragPath(std::move(frag)) {};
  unsigned int id_ = 0;
  void Use() const;
  void SetInt(std::string_view k_name, int value) const;
  void SetFloat(std::string_view k_name, float value) const;
  void SetVec2(std::string_view k_name, const glm::vec2& value) const;
  void SetVec3(std::string_view k_name, const glm::vec3& value) const;
  void SetMat4(std::string_view k_name, const glm::mat4& value) const;
};

#endif
