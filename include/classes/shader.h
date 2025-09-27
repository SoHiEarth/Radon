#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>
#include <utility>

class Shader {
public:
  const std::string kVertexPath, kFragmentPath;
  unsigned int id_ = 0;
  void Use() const;
  void SetInt(std::string_view /*name*/, int /*value*/) const;
  void SetFloat(std::string_view /*name*/, float /*value*/) const;
  void SetVec2(std::string_view /*name*/, const glm::vec2& /*value*/) const;
  void SetVec3(std::string_view /*name*/, const glm::vec3& /*value*/) const;
  void SetMat4(std::string_view /*name*/, const glm::mat4& /*value*/) const;
  Shader(std::string_view vert, std::string_view frag) : kVertexPath(vert), kFragmentPath(frag) {};
};

#endif
