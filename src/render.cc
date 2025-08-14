// This should always be at top
#include <glad/glad.h>
// Ok continue

#include <GLFW/glfw3.h>
#include <classes/camera.h>
#include <classes/light.h>
#include <classes/material.h>
#include <classes/shader.h>
#include <engine/devgui.h>
#include <engine/filesystem.h>
#include <engine/global.h>
#include <engine/render.h>
#include <fmt/core.h>
#include <imgui.h>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <vector>

#define TEXT_COLOR_RED ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define GUI_DRAG_STEP 0.1F
#define IMAGE_SIZE ImVec2(100, 100)
#define CAMERA_FOV 60.0F
#define CAMERA_NEAR_PLANE 0.1F
#define CAMERA_FAR_PLANE 100.0F

enum : std::uint8_t { kNumColorbuffers = 2, kDefaultBloomAmount = 10, kPingPongCount = 2, kDefaultExposure = 4 };
using ImGui::TextColored;

std::vector<DirectionalLight *> r::g_directional_lights;
std::vector<PointLight *> r::g_point_lights;
std::vector<SpotLight *> r::g_spot_lights;
unsigned int g_vao, g_vbo;
unsigned int g_framebuffer, g_renderbuffer;
std::array<unsigned int, kNumColorbuffers> g_colorbuffers;
std::array<unsigned int, kPingPongCount> g_pingpong_framebuffer, g_pingpong_colorbuffer;
std::vector<unsigned int> g_attachments;
unsigned int g_screen_vao, g_screen_vbo;
Shader *g_screen_shader = nullptr, *g_screen_shader_blur;
float g_render_factor = 1, g_prev_render_factor = g_render_factor;
float g_screen_exposure = kDefaultExposure;
int g_bloom_amount = kDefaultBloomAmount;
bool g_bloom = true;
bool g_dithering = false, g_scanlines = false;
;
float g_pixelation, g_brightness, g_scanline_intensity;

const std::array<float, 32> kGVertices = {-0.5F, 0.5F,  0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 1.0F,
                                          -0.5F, -0.5F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F,
                                          0.5F,  0.5F,  0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 1.0F,
                                          0.5F,  -0.5F, 0.0F, 0.0F, 0.0F, 1.0F, 1.0F, 0.0F};

const std::array<float, 20> kGScreenVertices = {
    -1.0F, 1.0F, 0.0F, 0.0F, 1.0F, -1.0F, -1.0F, 0.0F, 0.0F, 0.0F,
    1.0F,  1.0F, 0.0F, 1.0F, 1.0F, 1.0F,  -1.0F, 0.0F, 1.0F, 0.0F,
};

void DrawRendererStatus() {
  if (dev::g_hud_enabled) {
    ImGui::Begin("Renderer Status");
    ImGui::Text("Directional light status: %d", static_cast<int>(r::g_directional_lights.size()));
    ImGui::Text("Point light count: %d", static_cast<int>(r::g_point_lights.size()));
    ImGui::Text("Spot light count: %d", static_cast<int>(r::g_spot_lights.size()));
    ImGui::DragFloat3("Camera Position", glm::value_ptr(Engine::g_camera.position_));
    ImGui::SeparatorText("Screen Shader");
    ImGui::InputFloat("Pixelation Factor", &g_pixelation, GUI_DRAG_STEP);
    ImGui::Checkbox("Dithering", &g_dithering);
    ImGui::InputFloat("Brightness Levels", &g_brightness, GUI_DRAG_STEP);
    ImGui::Checkbox("Scanlines", &g_scanlines);
    ImGui::InputFloat("Scanline Intensity", &g_scanline_intensity, GUI_DRAG_STEP);
    ImGui::InputFloat("Exposure", &g_screen_exposure, GUI_DRAG_STEP);
    ImGui::Checkbox("Bloom", &g_bloom);
    ImGui::InputInt("Bloom Amount", &g_bloom_amount);
    if (g_bloom_amount <= 0) {
      g_bloom_amount = 1;
    }
    ImGui::InputFloat("Render Factor", &g_render_factor, GUI_DRAG_STEP);
    if (g_render_factor < 1) {
      TextColored(TEXT_COLOR_RED, "Downscaling");
    }
    if (g_render_factor > 1) {
      TextColored(TEXT_COLOR_RED, "Upscaling");
    }
    if (g_render_factor <= 0) {
      g_render_factor = g_prev_render_factor;
    }
    ImGui::Text("Colorbuffer");
    for (unsigned int g_colorbuffer : g_colorbuffers) {
      ImGui::Image(g_colorbuffer, IMAGE_SIZE);
    }
    ImGui::Text("Ping Pong Colorbuffer");
    for (unsigned int g_colorbuffer : g_pingpong_colorbuffer) {
      ImGui::Image(g_colorbuffer, IMAGE_SIZE);
    }
    ImGui::End();
  }
}

void RecreateFramebuffer();
void FBSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, Engine::g_width, Engine::g_height);
  Engine::g_width = width;
  Engine::g_height = height;
  RecreateFramebuffer();
}

void r::Init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  Engine::g_window = glfwCreateWindow(Engine::g_width, Engine::g_height, "Metal", nullptr, nullptr);
  if (Engine::g_window == nullptr) {
    std::runtime_error("Failed to create GLFW window");
  }
  glfwMakeContextCurrent(Engine::g_window);
  glfwSetFramebufferSizeCallback(Engine::g_window, FBSizeCallback);
  glfwSwapInterval(1);

  if (0 == gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::runtime_error("Failed to initialize GLAD\n");
    glfwTerminate();
    return;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_MULTISAMPLE);
  glViewport(0, 0, Engine::g_width, Engine::g_height);
  glGenVertexArrays(1, &g_vao);
  glGenBuffers(1, &g_vbo);
  glBindVertexArray(g_vao);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kGVertices), kGVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));

  glGenVertexArrays(1, &g_screen_vao);
  glGenBuffers(1, &g_screen_vbo);
  glBindVertexArray(g_screen_vao);
  glBindBuffer(GL_ARRAY_BUFFER, g_screen_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kGScreenVertices), kGScreenVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

  RecreateFramebuffer();

  g_screen_shader_blur = f::LoadShader("screen_blur/vert.glsl", "screen_blur/frag.glsl");
  g_screen_shader_blur->Use();
  g_screen_shader_blur->SetInt("image", 0);

  g_screen_shader = f::LoadShader("screen_shader/vert.glsl", "screen_shader/frag.glsl");
  g_screen_shader->Use();
  g_screen_shader->SetInt("scene", 0);
  g_screen_shader->SetInt("blur", 1);

  fmt::print("Initialized OpenGL\n");
}

void r::Update() {
  if (Engine::g_window == nullptr) {
    return;
  }
  if (g_render_factor != g_prev_render_factor) {
    RecreateFramebuffer();
  }
  glViewport(0, 0, Engine::g_width / g_render_factor, Engine::g_height / g_render_factor);
  glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void r::Render() {
  // Bloom Start
  bool horizontal = true;
  bool first_iteration = true;
  g_screen_shader_blur->Use();
  for (unsigned int i = 0; std::cmp_less(i, g_bloom_amount); i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, g_pingpong_framebuffer[static_cast<size_t>(horizontal)]);
    g_screen_shader_blur->SetInt("horizontal", static_cast<int>(horizontal));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_iteration
                                     ? g_colorbuffers[1]
                                     : g_pingpong_colorbuffer[static_cast<size_t>(!horizontal)]);
    glBindVertexArray(g_screen_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    horizontal = !horizontal;
    if (first_iteration) {
      first_iteration = false;
    }
  }
  // Bloom End
  glViewport(0, 0, Engine::g_width, Engine::g_height);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  dev::Update();
  DrawRendererStatus();

  g_screen_shader->Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_colorbuffers[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_pingpong_colorbuffer[static_cast<size_t>(!horizontal)]);
  g_screen_shader->SetInt("bloom", static_cast<int>(g_bloom));
  g_screen_shader->SetFloat("exposure", g_screen_exposure);
  g_screen_shader->SetFloat("pixelation_factor", g_pixelation);
  g_screen_shader->SetInt("enable_dithering", static_cast<int>(g_dithering));
  g_screen_shader->SetFloat("brightness_levels", g_brightness);
  g_screen_shader->SetInt("enable_scanlines", static_cast<int>(g_scanlines));
  g_screen_shader->SetFloat("scanline_intensity", g_scanline_intensity);
  glBindVertexArray(g_screen_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
  dev::Render();
  glfwSwapBuffers(Engine::g_window);
}

void r::Quit() {
  glDeleteVertexArrays(1, &g_vao);
  glDeleteBuffers(1, &g_vbo);
  if (g_renderbuffer != 0) {
    glDeleteRenderbuffers(1, &g_renderbuffer);
    g_renderbuffer = 0;
  }
  for (unsigned int &g_colorbuffer : g_colorbuffers) {
    if (g_colorbuffer != 0) {
      glDeleteTextures(1, &g_colorbuffer);
      g_colorbuffer = 0;
    }
  }
  if (g_framebuffer != 0) {
    glDeleteFramebuffers(1, &g_framebuffer);
    g_framebuffer = 0;
  }

  if (Engine::g_window != nullptr) {
    glfwDestroyWindow(Engine::g_window);
    Engine::g_window = nullptr;
  }
  glfwTerminate();
  fmt::print("Terminated OpenGL\n");
}

glm::mat4 GetTransform(const glm::vec3 &pos, const glm::vec2 &scale, float rot) {
  auto transform = glm::mat4(1.0F);
  transform = glm::translate(transform, pos);
  transform = glm::rotate(transform, glm::radians(rot), glm::vec3(0.0, 0.0, 1.0));
  transform = glm::scale(transform, glm::vec3(scale, 1));
  return transform;
}

void r::RenderTexture(const Material *material, const glm::vec3 &pos, const glm::vec2 &size,
                      float rot) {
  if (material == nullptr) {
    return;
  }
  if (!material->IsValid()) {
    return;
  }

  glm::mat4 model = GetTransform(pos, size, rot);
  glm::mat4 view = glm::translate(glm::mat4(1.0F), -Engine::g_camera.position_);
  glm::mat4 projection = glm::perspective(
      glm::radians(CAMERA_FOV),
      (static_cast<float>(Engine::g_width) / static_cast<float>(Engine::g_height)),
      CAMERA_NEAR_PLANE, CAMERA_FAR_PLANE);

  material->Bind();
  material->shader_->SetMat4("model", model);
  material->shader_->SetMat4("view", view);
  material->shader_->SetMat4("projection", projection);
  material->shader_->SetVec3("viewPos", Engine::g_camera.position_);

  material->shader_->SetInt("NUM_DIRECTIONAL_LIGHTS",
                            static_cast<int>(g_directional_lights.size()));
  for (int i = 0; i < g_directional_lights.size(); i++) {
    if (g_directional_lights[i] != nullptr) {
      g_directional_lights[i]->SetUniforms(material->shader_, i);
    }
  }

  material->shader_->SetInt("NUM_POINT_LIGHTS", static_cast<int>(g_point_lights.size()));
  for (int i = 0; i < g_point_lights.size(); i++) {
    if (g_point_lights[i] != nullptr) {
      g_point_lights[i]->SetUniforms(material->shader_, i);
    }
  }

  material->shader_->SetInt("NUM_SPOT_LIGHTS", static_cast<int>(g_spot_lights.size()));
  for (int i = 0; i < g_spot_lights.size(); i++) {
    if (g_spot_lights[i] != nullptr) {
      g_spot_lights[i]->SetUniforms(material->shader_, i);
    }
  }

  glBindVertexArray(g_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void RecreateFramebuffer() {
  if (!g_attachments.empty()) {
    g_attachments.clear();
  }
  if (g_renderbuffer != 0) {
    glDeleteRenderbuffers(1, &g_renderbuffer);
    g_renderbuffer = 0;
  }
  for (unsigned int &g_colorbuffer : g_colorbuffers) {
    if (g_colorbuffer != 0) {
      glDeleteTextures(1, &g_colorbuffer);
      g_colorbuffer = 0;
    }
  }
  if (g_framebuffer != 0) {
    glDeleteFramebuffers(1, &g_framebuffer);
    g_framebuffer = 0;
  }

  int framebuffer_width = Engine::g_width / g_render_factor;
  int framebuffer_height = Engine::g_height / g_render_factor;

  glGenFramebuffers(1, &g_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
  glGenTextures(kNumColorbuffers, g_colorbuffers.data());
  for (unsigned int i = 0; i < kNumColorbuffers; i++) {
    glBindTexture(GL_TEXTURE_2D, g_colorbuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, framebuffer_width, framebuffer_height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                           g_colorbuffers[i], 0);
    g_attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
  }

  glGenRenderbuffers(1, &g_renderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, g_renderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebuffer_width, framebuffer_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_renderbuffer);
  glDrawBuffers(2, g_attachments.data());
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::runtime_error("Failed to recreate framebuffer!\n");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glGenFramebuffers(2, g_pingpong_framebuffer.data());
  glGenTextures(2, g_pingpong_colorbuffer.data());
  for (unsigned int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, g_pingpong_framebuffer[i]);
    glBindTexture(GL_TEXTURE_2D, g_pingpong_colorbuffer[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, framebuffer_width, framebuffer_height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           g_pingpong_colorbuffer[i], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::runtime_error("Failed to recreate pingpong framebuffer!");
    }
  }
  g_prev_render_factor = g_render_factor;
}
