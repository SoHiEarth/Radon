#ifndef ENGINE_RENDER_H
#define ENGINE_RENDER_H

#include <classes/material.h>
#include <classes/shader.h>
#include <glm/glm.hpp>

namespace r {
  void Init();
  void Update();
  void Render();
  void Quit();
  void RenderTexture(const Material* material, const Shader* shader,
      const glm::vec3& pos, const glm::vec2& size, float rot);
}

#endif
