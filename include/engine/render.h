#ifndef RENDER_H
#define RENDER_H

#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class GLFWwindow;
class Camera;
class Material;
class DirectionalLight;
class PointLight;
class SpotLight;

enum : std::uint8_t {
  kDefaultRenderFactor = 1,
};

struct RenderSettings {
  float render_factor_ = kDefaultRenderFactor;
};

struct FramebufferCreateInfo {
  int num_colorbuffers_ = 0;
  int level_ = 0;
  int colorbuffer_format_ = 0;
  int width_ = 0;
  int height_ = 0;
  int border_ = 0;
  int format_ = 0;
  int type_ = 0;
  bool same_colorbuffer_attachment_ = false;
  bool create_renderbuffer_ = true;
};

struct Framebuffer {
  int width_ = 0, height_ = 0;
  unsigned int framebuffer_ = 0;
  unsigned int renderbuffer_ = 0;
  std::vector<unsigned int> colorbuffers_{};
  std::vector<unsigned int> attachments_{};
};

enum class RenderDrawMode : std::uint8_t {
  kFill = 0,
  kLine = 1,
  kPoint = 2
};

namespace render {
extern std::vector<std::shared_ptr<DirectionalLight>> g_directional_lights;
extern std::vector<std::shared_ptr<PointLight>> g_point_lights;
extern std::vector<std::shared_ptr<SpotLight>> g_spot_lights;
extern RenderSettings g_render_settings;
extern GLFWwindow* g_window;
extern int g_width;
extern int g_height;
extern Camera g_camera;

void Init();
void Update();
void Render();
void Quit();
void RenderTexture(std::shared_ptr<Material>, const glm::vec3&, const glm::vec2&, const glm::vec3&);
Framebuffer CreateFramebuffer(const FramebufferCreateInfo&);
void DeleteFramebuffer(Framebuffer&);

void SetRenderDrawMode(const RenderDrawMode&);
const RenderDrawMode GetRenderDrawMode();

void g_add_light(std::shared_ptr<DirectionalLight>);
void g_add_light(std::shared_ptr<PointLight>);
void g_add_light(std::shared_ptr<SpotLight>);
void g_remove_light(std::shared_ptr<DirectionalLight>);
void g_remove_light(std::shared_ptr<PointLight>);
void g_remove_light(std::shared_ptr<SpotLight>);
}  // namespace render

#endif  // RENDER_H