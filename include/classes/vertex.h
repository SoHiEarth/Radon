#ifndef VERTEX_H
#define VERTEX_H
#define MAX_BONE_INFLUENCE 4
#include <array>
#include <glm/glm.hpp>
struct Vertex {
  glm::vec3 position_, normal_, tangent_, bitangent_;
  glm::vec2 texcoords_;
  std::array<int, MAX_BONE_INFLUENCE> m_bone_ids_;
  std::array<float, MAX_BONE_INFLUENCE> m_weights_;
};
#endif
