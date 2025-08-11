#ifndef ENGINE_RENDER_H
#define ENGINE_RENDER_H

#include <classes/material.h>
#include <glm/glm.hpp>

class DirectionalLight;
class PointLight;
class SpotLight;

namespace r {
  void Init();
  void Update();
  void Render();
  void Quit();
  void RenderTexture(const Material* material, const glm::vec3& pos, const glm::vec2& size, float rot);
  void AddDirectionalLight(DirectionalLight* light);
  void AddPointLight(PointLight* light);
  void AddSpotLight(SpotLight* light);
  void RemoveDirectionalLight(DirectionalLight* target);
  void RemovePointLight(PointLight* target);
  void RemoveSpotLight(SpotLight* target);
}

#endif
