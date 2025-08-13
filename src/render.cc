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
enum : std::uint8_t { kNumColorbuffers = 2, kDefaultBloomAmount = 10, kPingPongCount = 2 };
using ImGui::TextColored;

DirectionalLight *r::g_directional_light = nullptr;
std::vector<PointLight *> r::g_point_lights;
std::vector<SpotLight *> r::g_spot_lights;
unsigned int g_vao, g_vbo, g_ebo;
unsigned int g_framebuffer, g_renderbuffer;
std::array<unsigned int, kNumColorbuffers> g_colorbuffers;
std::array<unsigned int, kPingPongCount> g_pingpong_framebuffer, g_pingpong_colorbuffer;
std::vector<unsigned int> g_attachments;
unsigned int g_screen_vao, g_screen_vbo;
Shader *g_screen_shader = nullptr, *g_screen_shader_blur;
float g_render_factor = 4, g_prev_render_factor;
float g_screen_exposure = 1.0;
int g_bloom_amount = kDefaultBloomAmount;
bool g_bloom = true, g_antialias = true;

const std::array<unsigned int, 6> kGIndices = {0, 1, 3, 1, 2, 3};
const std::array<float, 32> kGVertices = {0.5F,  0.5F,  0.0F, 0.0F, 0.0F, -1.0F, 1.0F, 1.0F,
                                          0.5F,  -0.5F, 0.0F, 0.0F, 0.0F, -1.0F, 1.0F, 0.0F,
                                          -0.5F, -0.5F, 0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F,
                                          -0.5F, 0.5F,  0.0F, 0.0F, 0.0F, -1.0F, 0.0F, 1.0F};
const std::array<float, 24> kGScreenVertices = {-1.0F, 1.0F,  0.0F, 1.0F, -1.0F, -1.0F, 0.0F, 0.0F,
                                                1.0F,  -1.0F, 1.0F, 0.0F, -1.0F, 1.0F,  0.0F, 1.0F,
                                                1.0F,  -1.0F, 1.0F, 0.0F, 1.0F,  1.0F,  1.0F, 1.0F};

void RecreateFramebuffer();
void FBSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
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
  glEnable(GL_MULTISAMPLE);
  glViewport(0, 0, Engine::g_width, Engine::g_height);
  glGenVertexArrays(1, &g_vao);
  glGenBuffers(1, &g_vbo);
  glGenBuffers(1, &g_ebo);
  glBindVertexArray(g_vao);
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kGVertices), kGVertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kGIndices), kGIndices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glGenVertexArrays(1, &g_screen_vao);
  glGenBuffers(1, &g_screen_vbo);
  glBindVertexArray(g_screen_vao);
  glBindBuffer(GL_ARRAY_BUFFER, g_screen_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(kGScreenVertices), kGScreenVertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) nullptr);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  RecreateFramebuffer();
  g_prev_render_factor = g_render_factor;

  g_screen_shader = f::LoadShader("screen_shader/vert.glsl", "screen_shader/frag.glsl");
  g_screen_shader->Use();
  g_screen_shader->SetInt("scene", 0);
  g_screen_shader->SetInt("blur", 1);

  g_screen_shader_blur = f::LoadShader("screen_blur/vert.glsl", "screen_blur/frag.glsl");
  g_screen_shader_blur->Use();
  g_screen_shader_blur->SetInt("image", 0);

  fmt::print("Initialized OpenGL\n");
}

void r::Update() {
  if (Engine::g_window == nullptr) {
    return;
  }
  if (g_render_factor != g_prev_render_factor) {
    RecreateFramebuffer();
    g_prev_render_factor = g_render_factor;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
  glViewport(0, 0, Engine::g_width / g_render_factor, Engine::g_height / g_render_factor);
  if (g_directional_light != nullptr) {
    glClearColor(g_directional_light->ambient_->x, g_directional_light->ambient_->y,
                 g_directional_light->ambient_->z, 1.0F);
  } else {
    glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void r::Render() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  // glViewport(0, 0, Engine::g_width, Engine::g_height);

  dev::Update();
  if (dev::g_hud_enabled) {
    ImGui::Begin("Renderer Status");
    ImGui::Text("Directional light status: %d", static_cast<int>(g_directional_light != nullptr));
    ImGui::Text("Point light count: %d", static_cast<int>(g_point_lights.size()));
    ImGui::Text("Spot light count: %d", static_cast<int>(g_spot_lights.size()));
    ImGui::DragFloat3("Camera Position", glm::value_ptr(Engine::g_camera.position_));
    ImGui::SeparatorText("Screen Shader");
    ImGui::InputFloat("Exposure", &g_screen_exposure, 0.1F);
    ImGui::Checkbox("Anti-Alias", &g_antialias);
    ImGui::Checkbox("Bloom", &g_bloom);
    ImGui::InputInt("Bloom Amount", &g_bloom_amount);
    if (g_bloom_amount <= 0) {
      g_bloom_amount = 1;
    }
    ImGui::InputFloat("Render Factor", &g_render_factor, 0.5F);
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
      ImGui::Image(g_colorbuffer, ImVec2(100, 100));
    }
    ImGui::Text("Ping Pong Colorbuffer");
    for (unsigned int g_colorbuffer : g_pingpong_colorbuffer) {
      ImGui::Image(g_colorbuffer, ImVec2(100, 100));
    }
    ImGui::End();
  }

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
    glDrawArrays(GL_TRIANGLES, 0, 6);
    horizontal = !horizontal;
    if (first_iteration) {
      first_iteration = false;
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if (g_antialias) {
    glEnable(GL_MULTISAMPLE);
  } else {
    glDisable(GL_MULTISAMPLE);
  }
  glViewport(0, 0, Engine::g_width, Engine::g_height);
  glClearColor(1.0F, 1.0F, 1.0F, 1.0F);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  g_screen_shader->Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_colorbuffers[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, g_pingpong_colorbuffer[static_cast<size_t>(!horizontal)]);
  g_screen_shader->SetInt("bloom", static_cast<int>(g_bloom));
  g_screen_shader->SetFloat("exposure", g_screen_exposure);
  glBindVertexArray(g_screen_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  dev::Render();
  glfwSwapBuffers(Engine::g_window);
}

void r::Quit() {
  glDeleteVertexArrays(1, &g_vao);
  glDeleteBuffers(1, &g_vbo);
  glDeleteBuffers(1, &g_ebo);
  glDeleteRenderbuffers(1, &g_renderbuffer);
  glDeleteFramebuffers(1, &g_framebuffer);
  if (Engine::g_window != nullptr) {
    glfwDestroyWindow(Engine::g_window);
    Engine::g_window = nullptr;
  }
  glfwTerminate();
  fmt::print("Terminated OpenGL\n");
}

glm::mat4 GetTransform(const glm::vec3 &pos, const glm::vec2 &scale, float rot) {
  auto transform = glm::mat4(1.0F);
  transform = glm::rotate(transform, glm::radians(rot), glm::vec3(0.0, 0.0, 1.0));
  transform = glm::translate(transform, pos);
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
  glm::mat4 view = glm::translate(glm::mat4(1.0F), Engine::g_camera.position_);
  glm::mat4 projection = glm::perspective(
      glm::radians(60.0F),
      (static_cast<float>(Engine::g_width) / static_cast<float>(Engine::g_height)), 0.1F, 100.0F);

  material->Bind();
  material->shader_->SetMat4("model", model);
  material->shader_->SetMat4("view", view);
  material->shader_->SetVec3("viewPos", Engine::g_camera.position_);
  material->shader_->SetMat4("projection", projection);

  if (g_directional_light != nullptr) {
    g_directional_light->SetUniforms(material->shader_);
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
  glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                           g_colorbuffers[i], 0);
    g_attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
  }

  glGenRenderbuffers(1, &g_renderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, g_renderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, framebuffer_width,
                        framebuffer_height);

  glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                            g_renderbuffer);
  glDrawBuffers(2, g_attachments.data());
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::runtime_error("Failed to recreate framebuffer!\n");
    glDeleteRenderbuffers(1, &g_renderbuffer);
    glDeleteFramebuffers(1, &g_framebuffer);
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
}
