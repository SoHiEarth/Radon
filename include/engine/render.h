#ifndef RENDER_H
#define RENDER_H

#include <classes/camera.h>
#include <engine/interface.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

class GLFWwindow;
class Shader;
class Mesh;
class Model;
class Material;
class DirectionalLight;
class PointLight;
class SpotLight;

enum : std::uint16_t {
  kDefaultRenderFactor = 1,
  kDefaultWindowWidth = 800,
  kDefaultWindowHeight = 600
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
  std::vector<unsigned int> colorbuffers_;
  std::vector<unsigned int> attachments_;
};

enum class RenderDrawMode : std::uint8_t { kFill = 0, kLine = 1, kPoint = 2 };

class IRenderer : public Interface {
private:
  std::vector<DirectionalLight*> directional_lights;
  std::vector<PointLight*> point_lights;
  std::vector<SpotLight*> spot_lights;
  RenderSettings settings{};
  GLFWwindow* window = nullptr;
  int width = kDefaultWindowWidth;
  int height = kDefaultWindowHeight;
  Camera camera{};

protected:
  const char* name() override {
    return "Renderer";
  }
  void i_Init() override;
  void i_Update();
  void i_Render();
  void i_Quit() override;

public:
  void DrawMesh(const Mesh* /*mesh*/, const Shader* /*shader*/, const glm::vec3& /*pos*/,
                const glm::vec2& /*size*/, const glm::vec3& /*rot*/);

  void DrawModel(const Model* /*model*/, const Shader* /*shader*/, const glm::vec3& /*pos*/,
                 const glm::vec2& /*size*/, const glm::vec3& /*rot*/);

  void RenderTexture(const Material*& /*material*/, const glm::vec3& /*pos*/,
                     const glm::vec2& /*size*/, const glm::vec3& /*rot*/);

  Framebuffer CreateFramebuffer(const FramebufferCreateInfo& /*create_info*/);
  void DeleteFramebuffer(Framebuffer& /*framebuffer*/);
  GLFWwindow* GetWindow() {
    return window;
  }
  Camera& GetCamera() {
    return camera;
  }
  RenderSettings& GetSettings() {
    return settings;
  }
  std::vector<DirectionalLight*>& GetDirectionalLights() {
    return directional_lights;
  }
  std::vector<PointLight*>& GetPointLights() {
    return point_lights;
  }
  std::vector<SpotLight*>& GetSpotLights() {
    return spot_lights;
  }
  int& GetWidth() {
    return width;
  }
  int& GetHeight() {
    return height;
  }
  void SetRenderDrawMode(const RenderDrawMode& /*mode*/);
  RenderDrawMode GetRenderDrawMode();
  void GAddLight(DirectionalLight* /*light*/);
  void GAddLight(PointLight* /*light*/);
  void GAddLight(SpotLight* /*light*/);
  void GRemoveLight(const DirectionalLight* /*light*/);
  void GRemoveLight(const PointLight* /*light*/);
  void GRemoveLight(const SpotLight* /*light*/);
};

#endif  // RENDER_H
