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
  kDefaultExposure = 1,
  kDefaultBloom = 0,
  kDefaultBloomAmount = 10,
  kDefaultDithering = 0,
  kDefaultPixelation = 1,
  kDefaultScanlines = 0,
  kDefaultScanlinesIntensity = 1,
  kDefaultBrightness = 1
};

struct RenderSettings {
  float render_factor_ = kDefaultRenderFactor;
  float exposure_ = kDefaultExposure;
  bool bloom_ = static_cast<bool>(kDefaultBloom);
  int bloom_amount_ = kDefaultBloomAmount;
  bool dithering_ = static_cast<bool>(kDefaultDithering);
  float pixelation_ = kDefaultPixelation;
  bool scanlines_ = static_cast<bool>(kDefaultScanlines);
  float scanline_intensity_ = kDefaultScanlinesIntensity;
  float brightness_ = kDefaultBrightness;
};

struct FramebufferCreateInfo {
  int num_colorbuffers_ = 0;
  int level_ = 0;
  int colorbuffer_format_;
  int width_, height_;
  int border_;
  int format_, type_;
  bool same_colorbuffer_attachment_ = false;
  bool create_renderbuffer_ = true;
};

struct Framebuffer {
  int width_, height_;
  unsigned int framebuffer_;
  unsigned int renderbuffer_;
  std::vector<unsigned int> colorbuffers_;
  std::vector<unsigned int> attachments_;
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
void RenderTexture(std::shared_ptr<Material> /*material*/, const glm::vec3& /*pos*/,
                   const glm::vec2& /*size*/, const glm::vec3& /*rot*/);
Framebuffer CreateFramebuffer(FramebufferCreateInfo& create_info);
void DeleteFramebuffer(Framebuffer& framebuffer);

void g_add_light(std::shared_ptr<DirectionalLight> light);
void g_add_light(std::shared_ptr<PointLight> light);
void g_add_light(std::shared_ptr<SpotLight> light);
void g_remove_light(std::shared_ptr<DirectionalLight> light);
void g_remove_light(std::shared_ptr<PointLight> light);
void g_remove_light(std::shared_ptr<SpotLight> light);
}  // namespace render

#endif  // RENDER_H