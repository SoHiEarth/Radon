#ifndef RENDER_H
#define RENDER_H

#include <glm/glm.hpp>
#include <vector>

class Material;
class DirectionalLight;
class PointLight;
class SpotLight;

namespace r {
  extern DirectionalLight* directional_light;
  extern std::vector<PointLight*> point_lights;
  extern std::vector<SpotLight*> spot_lights;

  void Init();
  void Update();
  void Render();
  void Quit();
  void RenderTexture(const Material* material,
      const glm::vec3& pos, const glm::vec2& size, float rot);

  template <typename T> void AddLight(T* light) {
    if (light == nullptr) return;
    if constexpr (std::is_same_v<T, DirectionalLight>) {
      directional_light = light;
    }
    else if constexpr (std::is_same_v<T, PointLight>) {
      point_lights.push_back(light);
    }
    else if constexpr (std::is_same_v<T, SpotLight>) {
      spot_lights.push_back(light);
    }
  }

  template <typename T> void RemoveLight(T* light) {
    if (light == nullptr) return;
    if constexpr (std::is_same_v<T, DirectionalLight>) {
      directional_light = nullptr;
    }
    else if constexpr (std::is_same_v<T, PointLight>) {
      for (int i = 0; i < point_lights.size(); i++) {
        if (point_lights[i] == light) {
          point_lights.erase(point_lights.begin() + i);
        }
      }
    }
    else if constexpr (std::is_same_v<T, SpotLight>) {
      for (int i = 0; i< spot_lights.size(); i++) {
        if (spot_lights[i] == light) {
          spot_lights.erase(spot_lights.begin() + i);
        }
      }
    }
  }
}

#endif // RENDER_H
