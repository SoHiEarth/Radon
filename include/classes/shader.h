#pragma once
#include <glm/glm.hpp>

class Shader {
public:
  const char* kVertexPath;
  const char* kFragmentPath;
  unsigned int id_ = 0;
  void Use() const;
  void SetInt(const char*/*name*/, int /*value*/) const;
  void SetFloat(const char* /*name*/, float /*value*/) const;
  void SetVec2(const char* /*name*/, const glm::vec2& /*value*/) const;
  void SetVec3(const char* /*name*/, const glm::vec3& /*value*/) const;
  void SetMat4(const char* /*name*/, const glm::mat4& /*value*/) const;
  Shader(const char* vert, const char* frag) {
    kVertexPath = strcpy(new char[strlen(vert) + 1], vert);
    kFragmentPath = strcpy(new char[strlen(frag) + 1], frag);
  };
};