#ifndef RENDER_H
#define RENDER_H

#include <glm/glm.hpp>
#include <vector>

class GLFWwindow;
class Camera;
class Material;
class DirectionalLight;
class PointLight;
class SpotLight;

struct RenderSettings {
  float render_factor_ = 1;
  float exposure_ = 1.0F;
  bool bloom_ = false;
  int bloom_amount_ = 10;
  bool dithering_ = false;
  float pixelation_ = 1.0F;
  bool scanlines_ = false;
  float scanline_intensity_ = 1.0F;
  float brightness_ = 1.0F;
};

namespace render {
extern std::vector<DirectionalLight*> g_directional_lights;
extern std::vector<PointLight*> g_point_lights;
extern std::vector<SpotLight*> g_spot_lights;
extern RenderSettings g_render_settings;
extern GLFWwindow* g_window;
extern int g_width;
extern int g_height;
extern Camera g_camera;

void Init();
void Update();
void Render();
void Quit();
void RenderTexture(const Material*, const glm::vec3&, const glm::vec2&, const float&);

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
}  // namespace render

#endif  // RENDER_H
