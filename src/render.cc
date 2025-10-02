// This should always be at top
#include <glad/glad.h>
// Ok continue

#include <GLFW/glfw3.h>
#include <classes/camera.h>
#include <classes/light.h>
#include <classes/material.h>
#include <classes/mesh.h>
#include <classes/shader.h>
#include <classes/texture.h>
#include <engine/debug.h>
#include <engine/devgui.h>
#include <engine/input.h>
#include <engine/io.h>
#include <engine/render.h>
#include <fmt/core.h>
#include <imgui.h>
#include <array>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <vector>

constexpr ImVec4 kTextColorRed = ImVec4(1.0F, 0.0F, 0.0F, 1.0F);
constexpr float kGuiDragStep = 0.1F;
constexpr ImVec2 kImageSize = ImVec2(100, 100);
constexpr float kDefaultCameraFov = 60.0F;
constexpr float kCameraNearPlane = 0.1F;
constexpr float kCameraFarPlane = 100.0F;
constexpr float kDefaultCameraSpeed = 0.1;
float g_camera_speed = kDefaultCameraSpeed;
static inline void IfNoHUD(const std::function<void()>& fn) {
  if (!IGui::Get<IGui>().GetHud()) {
    fn();
  }
}

using ImGui::TextColored;
float g_camera_fov = kDefaultCameraFov;
unsigned int g_vao, g_vbo;
Framebuffer g_framebuffer;
unsigned int g_screen_vao, g_screen_vbo;
Shader* g_screen_shader = nullptr;
Shader* g_screen_shader_blur = nullptr;
float g_prev_render_factor = IRenderer::Get<IRenderer>().GetSettings().render_factor_;
ImVec2 g_last_viewport_size = ImVec2(0.0F, 0.0F);

const std::array<float, 32> kGVertices = {-0.5F, 0.5F,  0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 1.0F,
                                          -0.5F, -0.5F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F,
                                          0.5F,  0.5F,  0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F,
                                          0.5F,  -0.5F, 0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.0F};

const std::array<float, 20> kGScreenVertices = {
    -1.0F, 1.0F, 0.0F, 0.0F, 1.0F, -1.0F, -1.0F, 0.0F, 0.0F, 0.0F,
    1.0F,  1.0F, 0.0F, 1.0F, 1.0F, 1.0F,  -1.0F, 0.0F, 1.0F, 0.0F,
};
void DrawRendererStatus();
void RecreateFramebuffer();
void FBSizeCallback(GLFWwindow* window, int width, int height);

void IRenderer::IInit() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  window_ = glfwCreateWindow(width_, height_, "Radon Engine", nullptr, nullptr);
  if (window_ == nullptr) {
    const char* error_desc;
    glfwGetError(&error_desc);
    IDebug::Throw(std::format("Failed to create GLFW window. {}", error_desc));
  }
  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, FBSizeCallback);
  glfwSwapInterval(1);
  if (0 == gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    IDebug::Throw("Failed to initialize GLAD");
    glfwTerminate();
    return;
  }
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_MULTISAMPLE);
  glViewport(0, 0, width_, height_);
  glGenVertexArrays(1, &g_vao);
  glGenBuffers(1, &g_vbo);
  glBindVertexArray(g_vao);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kGVertices), kGVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(6 * sizeof(float)));
  glGenVertexArrays(1, &g_screen_vao);
  glGenBuffers(1, &g_screen_vbo);
  glBindVertexArray(g_screen_vao);
  glBindBuffer(GL_ARRAY_BUFFER, g_screen_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kGScreenVertices), kGScreenVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void*>(3 * sizeof(float)));
  RecreateFramebuffer();
  g_screen_shader =
      IIO::LoadShader(IIO::Get<IIO>().GetEngineDirectory() + "/screen_shader/vert.glsl",
                                 IIO::Get<IIO>().GetEngineDirectory() + "/screen_shader/frag.glsl");
  g_screen_shader->Use();
  g_screen_shader->SetInt("scene", 0);

  IInput::AddHook({GLFW_KEY_W, ButtonState::kHold}, []() {
    IfNoHUD([]() { IRenderer::Get<IRenderer>().camera_.position_.z -= g_camera_speed; });
  });
  IInput::AddHook({GLFW_KEY_S, ButtonState::kHold}, []() {
    IfNoHUD([]() { IRenderer::Get<IRenderer>().camera_.position_.z += g_camera_speed; });
  });
  IInput::AddHook({GLFW_KEY_A, ButtonState::kHold}, []() {
    IfNoHUD([]() { IRenderer::Get<IRenderer>().camera_.position_.x -= g_camera_speed; });
  });
  IInput::AddHook({GLFW_KEY_D, ButtonState::kHold}, []() {
    IfNoHUD([]() { IRenderer::Get<IRenderer>().camera_.position_.x += g_camera_speed; });
  });
  IInput::AddHook({GLFW_KEY_E, ButtonState::kHold}, []() {
    IfNoHUD([]() { IRenderer::Get<IRenderer>().camera_.position_.y += g_camera_speed; });
  });
  IInput::AddHook({GLFW_KEY_Q, ButtonState::kHold}, []() {
    IfNoHUD([]() { IRenderer::Get<IRenderer>().camera_.position_.y -= g_camera_speed; });
  });
}

void IRenderer::IUpdate() {
  if (window_ == nullptr) {
    return;
  }
  width_ = std::max(1, width_);
  height_ = std::max(1, height_);
  if (settings_.render_factor_ != g_prev_render_factor) {
    RecreateFramebuffer();
  }
  glViewport(0, 0, g_framebuffer.width_, g_framebuffer.height_);
  glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer.framebuffer_);
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void IRenderer::IRender() {
  glViewport(0, 0, width_, height_);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  IGui::Get<IGui>().Update();
  DrawRendererStatus();
  if (IGui::Get<IGui>().GetHud()) {
    ImGui::Begin("Viewport");
    auto image_size = ImGui::GetContentRegionAvail();
#ifdef __linux__
    ImGui::Image(g_framebuffer.colorbuffers_[0], image_size);
#else
    ImGui::Image(g_framebuffer.colorbuffers_[0], image_size, ImVec2(0, 1), ImVec2(1, 0));
#endif
    ImGui::End();
  } else {
    g_screen_shader->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_framebuffer.colorbuffers_[0]);
    glBindVertexArray(g_screen_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
  }
  IGui::Get<IGui>().Render();
  if (window_ != nullptr) {
    glfwSwapBuffers(window_);
  } else {
    IDebug::Warning("Window is null in IRenderer::i_Render");
    state_ = InterfaceState::kError;
  }
}

void IRenderer::IQuit() {
  glDeleteVertexArrays(1, &g_vao);
  glDeleteBuffers(1, &g_vbo);
  DeleteFramebuffer(g_framebuffer);
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
    window_ = nullptr;
  }
  glfwTerminate();
}

/////////////////////////////
/// Helper Functions etc. ///
/////////////////////////////

void DrawRendererStatus() {
  if (IGui::Get<IGui>().GetHud()) {
    ImGui::Begin("IRenderer");
    ImGui::Text("Directional light count: %d",
                static_cast<int>(IRenderer::Get<IRenderer>().GetDirectionalLights().size()));
    ImGui::Text("Point light count: %d",
                static_cast<int>(IRenderer::Get<IRenderer>().GetPointLights().size()));
    ImGui::Text("Spot light count: %d",
                static_cast<int>(IRenderer::Get<IRenderer>().GetSpotLights().size()));
    ImGui::DragFloat3("Camera Position",
                      glm::value_ptr(IRenderer::Get<IRenderer>().GetCamera().position_));
    ImGui::DragFloat("Camera Field of View", &g_camera_fov);
    ImGui::DragFloat("Camera Speed", &g_camera_speed, 0.01F, 0.01F, 10.0F);
    ImGui::SeparatorText("Screen Shader");
    ImGui::InputFloat("Render Factor", &IRenderer::Get<IRenderer>().GetSettings().render_factor_,
                      kGuiDragStep);
    ImGui::Text("Colorbuffer");
    for (unsigned int colorbuffer : g_framebuffer.colorbuffers_) {
      ImGui::Image(colorbuffer, kImageSize);
    }
    ImGui::End();
  }
}

static glm::mat4 GetTransform(const glm::vec3& pos, const glm::vec2& scale, const glm::vec3& rot) {
  auto transform = glm::mat4(1.0F);
  transform = glm::translate(transform, pos);
  transform = glm::rotate(transform, rot.z, glm::vec3(0.0F, 0.0F, 1.0F));
  transform = glm::rotate(transform, rot.y, glm::vec3(0.0F, 1.0F, 0.0F));
  transform = glm::rotate(transform, rot.x, glm::vec3(1.0F, 0.0F, 0.0F));
  transform = glm::scale(transform, glm::vec3(scale, 1));
  return transform;
}

void IRenderer::DrawMesh(const Mesh* mesh, const Shader* shader, const glm::vec3& pos,
                         const glm::vec2& size, const glm::vec3& rot) {
  if (mesh == nullptr) {
    return;
  }
  if (shader == nullptr) {
    return;
  }
  glm::mat4 model = GetTransform(pos, size, rot);
  glm::mat4 view = glm::translate(glm::mat4(1.0F), -IRenderer::Get<IRenderer>().camera_.position_);
  glm::mat4 projection = glm::perspective(
      glm::radians(g_camera_fov),
      (static_cast<float>(g_framebuffer.width_) / static_cast<float>(g_framebuffer.height_)),
      kCameraNearPlane, kCameraFarPlane);
  if (shader == nullptr) {
    return;
  }
  shader->Use();
  shader->SetMat4("model", model);
  shader->SetMat4("view", view);
  shader->SetMat4("projection", projection);
  shader->SetVec3("viewPos", IRenderer::Get<IRenderer>().camera_.position_);
  shader->SetInt("NUM_DIRECTIONAL_LIGHTS",
                 static_cast<int>(IRenderer::GetDirectionalLights().size()));
  for (int i = 0; i < IRenderer::GetDirectionalLights().size(); i++) {
    if (IRenderer::GetDirectionalLights()[i] != nullptr) {
      IRenderer::GetDirectionalLights()[i]->SetUniforms(shader, i);
    }
  }
  shader->SetInt("NUM_POINT_LIGHTS", static_cast<int>(IRenderer::GetPointLights().size()));
  for (int i = 0; i < IRenderer::GetPointLights().size(); i++) {
    if (IRenderer::GetPointLights()[i] != nullptr) {
      IRenderer::GetPointLights()[i]->SetUniforms(shader, i);
    }
  }
  shader->SetInt("NUM_SPOT_LIGHTS", static_cast<int>(IRenderer::GetSpotLights().size()));
  for (int i = 0; i < IRenderer::GetSpotLights().size(); i++) {
    if (IRenderer::GetSpotLights()[i] != nullptr) {
      IRenderer::GetSpotLights()[i]->SetUniforms(shader, i);
    }
  }

  unsigned int diffuse_number = 1;
  unsigned int specular_number = 1;
  for (unsigned int i = 0; i < mesh->textures_.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = mesh->textures_[i]->name_;
    if (name == "texture_diffuse") {
      number = std::to_string(diffuse_number++);
    } else if (name == "texture_specular") {
      number = std::to_string(specular_number++);
    }
    shader->SetInt(("material." + name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, mesh->textures_[i]->id_);
  }

  glBindVertexArray(mesh->vao_);
  glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh->indices_.size()), GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);
}

void IRenderer::DrawModel(const Model* model, const Shader* shader, const glm::vec3& pos,
                          const glm::vec2& size, const glm::vec3& rot) {
  if (model == nullptr) {
    return;
  }
  for (const auto& mesh : model->meshes_) {
    DrawMesh(mesh, shader, pos, size, rot);
  }
}

void FBSizeCallback(GLFWwindow* window, int width, int height) {
  IRenderer::Get<IRenderer>().GetWidth() = width;
  IRenderer::Get<IRenderer>().GetHeight() = height;
  RecreateFramebuffer();
  glViewport(0, 0, IRenderer::Get<IRenderer>().GetWidth(), IRenderer::Get<IRenderer>().GetHeight());
}

Framebuffer IRenderer::CreateFramebuffer(const FramebufferCreateInfo& create_info) {
  Framebuffer framebuffer;
  framebuffer.width_ = create_info.width_;
  framebuffer.height_ = create_info.height_;
  glGenFramebuffers(1, &framebuffer.framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.framebuffer_);
  framebuffer.colorbuffers_.resize(create_info.num_colorbuffers_);
  glGenTextures(create_info.num_colorbuffers_, framebuffer.colorbuffers_.data());
  for (unsigned int i = 0; std::cmp_less(i, create_info.num_colorbuffers_); i++) {
    glBindTexture(GL_TEXTURE_2D, framebuffer.colorbuffers_[i]);
    glTexImage2D(GL_TEXTURE_2D, create_info.level_, create_info.colorbuffer_format_,
                 create_info.width_, create_info.height_, create_info.border_, create_info.format_,
                 create_info.type_, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (create_info.same_colorbuffer_attachment_) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                             framebuffer.colorbuffers_[i], 0);
      if (framebuffer.attachments_.empty()) {
        framebuffer.attachments_.push_back(GL_COLOR_ATTACHMENT0);
      }
    } else {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                             framebuffer.colorbuffers_[i], 0);
      framebuffer.attachments_.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
  }

  if (create_info.create_renderbuffer_) {
    glGenRenderbuffers(1, &framebuffer.renderbuffer_);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.renderbuffer_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, create_info.width_,
                          create_info.height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                              framebuffer.renderbuffer_);
    glDrawBuffers(framebuffer.attachments_.size(), framebuffer.attachments_.data());
  }
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    IDebug::Throw("Failed to create framebuffer!");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return framebuffer;
}

void IRenderer::DeleteFramebuffer(Framebuffer& framebuffer) {
  if (!framebuffer.attachments_.empty()) {
    framebuffer.attachments_.clear();
  }
  if (framebuffer.renderbuffer_ != 0) {
    glDeleteRenderbuffers(1, &framebuffer.renderbuffer_);
    framebuffer.renderbuffer_ = 0;
  }
  for (unsigned int& colorbuffer : framebuffer.colorbuffers_) {
    if (colorbuffer != 0) {
      glDeleteTextures(1, &colorbuffer);
      colorbuffer = 0;
    }
  }
  if (framebuffer.framebuffer_ != 0) {
    glDeleteFramebuffers(1, &framebuffer.framebuffer_);
    framebuffer.framebuffer_ = 0;
  }
}

void IRenderer::SetRenderDrawMode(const RenderDrawMode& mode) {
  switch (mode) {
    case RenderDrawMode::kFill:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
    case RenderDrawMode::kLine:
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      break;
    case RenderDrawMode::kPoint:
      glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
      break;
    default:
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      break;
  }
}

RenderDrawMode IRenderer::GetRenderDrawMode() {
  GLint mode;
  glGetIntegerv(GL_POLYGON_MODE, &mode);
  switch (mode) {
    case GL_FILL:
      return RenderDrawMode::kFill;
    case GL_LINE:
      return RenderDrawMode::kLine;
    case GL_POINT:
      return RenderDrawMode::kPoint;
    default:
      return RenderDrawMode::kFill;
  }
}

void RecreateFramebuffer() {
  IRenderer::DeleteFramebuffer(g_framebuffer);
  int framebuffer_width = std::max(1, IRenderer::Get<IRenderer>().GetWidth()) /
                          IRenderer::Get<IRenderer>().GetSettings().render_factor_;
  int framebuffer_height = std::max(1, IRenderer::Get<IRenderer>().GetHeight()) /
                           IRenderer::Get<IRenderer>().GetSettings().render_factor_;
  FramebufferCreateInfo create_info;
  create_info.num_colorbuffers_ = 2;
  create_info.level_ = 0;
  create_info.colorbuffer_format_ = GL_RGBA16F;
  create_info.width_ = framebuffer_width;
  create_info.height_ = framebuffer_height;
  create_info.border_ = 0;
  create_info.format_ = GL_RGBA;
  create_info.type_ = GL_FLOAT;
  create_info.same_colorbuffer_attachment_ = false;
  create_info.create_renderbuffer_ = true;
  g_framebuffer = IRenderer::CreateFramebuffer(create_info);
  g_prev_render_factor = IRenderer::Get<IRenderer>().GetSettings().render_factor_;
}

void IRenderer::GAddLight(DirectionalLight* light) {
  directional_lights_.push_back(light);
}

void IRenderer::GAddLight(PointLight* light) {
  point_lights_.push_back(light);
}

void IRenderer::GAddLight(SpotLight* light) {
  spot_lights_.push_back(light);
}

void IRenderer::GRemoveLight(const DirectionalLight* light) {
  for (int i = 0; i < directional_lights_.size(); i++) {
    if (directional_lights_.at(i) == light) {
      directional_lights_.erase(directional_lights_.begin() + i);
    }
  }
}

void IRenderer::GRemoveLight(const PointLight* light) {
  for (int i = 0; i < point_lights_.size(); i++) {
    if (point_lights_.at(i) == light) {
      point_lights_.erase(point_lights_.begin() + i);
    }
  }
}

void IRenderer::GRemoveLight(const SpotLight* light) {
  for (int i = 0; i < spot_lights_.size(); i++) {
    if (spot_lights_.at(i) == light) {
      spot_lights_.erase(spot_lights_.begin() + i);
    }
  }
}
