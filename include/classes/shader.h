#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>
#include <utility>

class Shader {
public:
  const std::string vertex_path_, fragment_path_;
  unsigned int id_ = 0;
  void Use() const;
  void SetInt(std::string_view, int) const;
  void SetFloat(std::string_view , float) const;
  void SetVec2(std::string_view, const glm::vec2&) const;
  void SetVec3(std::string_view, const glm::vec3&) const;
  void SetMat4(std::string_view, const glm::mat4&) const;
  Shader(std::string_view vert, std::string_view frag)
      : vertex_path_(vert), fragment_path_(frag) {};
};

#endif
