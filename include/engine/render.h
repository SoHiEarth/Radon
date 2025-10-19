#ifndef RENDER_H
#define RENDER_H

#include <classes/camera.h>
#include <engine/interface.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

struct GLFWwindow;
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
  std::vector<DirectionalLight*> directional_lights_;
  std::vector<PointLight*> point_lights_;
  std::vector<SpotLight*> spot_lights_;
  RenderSettings settings_{};
  GLFWwindow* window_ = nullptr;
  int width_ = kDefaultWindowWidth;
  int height_ = kDefaultWindowHeight;
  Camera camera_{};

protected:
  void IInit() override;
  void IUpdate() override;
  void IRender() override;
  void IQuit() override;

public:
  const char* Name() override {
    return "Renderer";
  }
  void DrawMesh(const Mesh* /*mesh*/, const Shader* /*shader*/, const glm::vec3& /*pos*/,
                const glm::vec2& /*size*/, const glm::vec3& /*rot*/);

  void DrawModel(const Model* /*model*/, const Shader* /*shader*/, const glm::vec3& /*pos*/,
                 const glm::vec2& /*size*/, const glm::vec3& /*rot*/);
  void DrawRendererStatus();
  Framebuffer CreateFramebuffer(const FramebufferCreateInfo& /*create_info*/);
  void DeleteFramebuffer(Framebuffer& /*framebuffer*/);
  void RecreateFramebuffer();
  GLFWwindow* GetWindow() {
    return window_;
  }
  Camera& GetCamera() {
    return camera_;
  }
  RenderSettings& GetSettings() {
    return settings_;
  }
  std::vector<DirectionalLight*>& GetDirectionalLights() {
    return directional_lights_;
  }
  std::vector<PointLight*>& GetPointLights() {
    return point_lights_;
  }
  std::vector<SpotLight*>& GetSpotLights() {
    return spot_lights_;
  }
  int& GetWidth() {
    return width_;
  }
  int& GetHeight() {
    return height_;
  }
  static void SetRenderDrawMode(const RenderDrawMode& /*mode*/);
  static RenderDrawMode GetRenderDrawMode();
  void AddLight(DirectionalLight* /*light*/);
  void AddLight(PointLight* /*light*/);
  void AddLight(SpotLight* /*light*/);
  void RemoveLight(const DirectionalLight* /*light*/);
  void RemoveLight(const PointLight* /*light*/);
  void RemoveLight(const SpotLight* /*light*/);
};

#endif  // RENDER_H
