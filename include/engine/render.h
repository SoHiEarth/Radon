#ifndef RENDER_H
#define RENDER_H

#include <glm/glm.hpp>
#include <vector>

class Material;
class DirectionalLight;
class PointLight;
class SpotLight;

namespace r {
extern std::vector<DirectionalLight*> g_directional_lights;
extern std::vector<PointLight*> g_point_lights;
extern std::vector<SpotLight*> g_spot_lights;

void Init();
void Extracted();
void Update();
void Render();
void Quit();
void RenderTexture(const Material* material, const glm::vec3& pos, const glm::vec2& size,
                   float rot);

template <typename T>
void AddLight(T* light) {
  if (light == nullptr) {
    return;
  }
  if constexpr (std::is_same_v<T, DirectionalLight>) {
    g_directional_lights.push_back(light);
  } else if constexpr (std::is_same_v<T, PointLight>) {
    g_point_lights.push_back(light);
  } else if constexpr (std::is_same_v<T, SpotLight>) {
    g_spot_lights.push_back(light);
  }
}

template <typename T>
void RemoveLight(T* light) {
  if (light == nullptr) {
    return;
  }
  if constexpr (std::is_same_v<T, DirectionalLight>) {
    for (int i = 0; i < g_directional_lights.size(); i++) {
      if (g_directional_lights[i] == light) {
        g_directional_lights.erase(g_directional_lights.begin() + i);
      }
    }
  } else if constexpr (std::is_same_v<T, PointLight>) {
    for (int i = 0; i < g_point_lights.size(); i++) {
      if (g_point_lights[i] == light) {
        g_point_lights.erase(g_point_lights.begin() + i);
      }
    }
  } else if constexpr (std::is_same_v<T, SpotLight>) {
    for (int i = 0; i < g_spot_lights.size(); i++) {
      if (g_spot_lights[i] == light) {
        g_spot_lights.erase(g_spot_lights.begin() + i);
      }
    }
  }
}
}  // namespace r

#endif  // RENDER_H
