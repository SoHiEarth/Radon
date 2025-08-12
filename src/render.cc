#include <glad/glad.h>
#include <engine/render.h>
#include <engine/global.h>
#include <engine/devgui.h>
#include <engine/filesystem.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <vector>
#include <classes/light.h>
#include <classes/camera.h>
#include <classes/material.h>
#include <classes/shader.h>

DirectionalLight* r::directional_light = nullptr;
std::vector<PointLight*> r::point_lights;
std::vector<SpotLight*> r::spot_lights;
unsigned int VAO, VBO, EBO, framebuffer, renderbuffer, colorbuffer;
unsigned int screen_vao, screen_vbo;
Shader* screen_shader = nullptr;
int render_factor = 4, prev_render_factor;

void RecreateFramebuffer();
void FBSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  Engine::width = width;
  Engine::height = height;
  RecreateFramebuffer();
}

float vertices[] = {
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f
};

unsigned int indices[] = {
  0, 1, 3,
  1, 2, 3
};

float screen_vertices[] = {
  -1.0f,  1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  0.0f, 0.0f,
   1.0f, -1.0f,  1.0f, 0.0f,
  -1.0f,  1.0f,  0.0f, 1.0f,
   1.0f, -1.0f,  1.0f, 0.0f,
   1.0f,  1.0f,  1.0f, 1.0f
};

void r::Init() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  Engine::width = 800;
  Engine::height = 600;
  Engine::window = glfwCreateWindow(Engine::width, Engine::height, "Metal", nullptr, nullptr);
  if (!Engine::window) {
    fmt::print("Failed to create GLFW window\n");
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(Engine::window);
  glfwSetFramebufferSizeCallback(Engine::window, FBSizeCallback);
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fmt::print("Failed to initialize GLAD\n");
    glfwTerminate();
    return;
  }

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, Engine::width, Engine::height);
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glGenVertexArrays(1, &screen_vao);
  glGenBuffers(1, &screen_vbo);
  glBindVertexArray(screen_vao);
  glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), screen_vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  RecreateFramebuffer();
  prev_render_factor = render_factor;

  screen_shader = f::LoadShader("screen_shader/vert.glsl", "screen_shader/frag.glsl");
  if (screen_shader == nullptr) {
    fmt::print("Failed to load screen shader.\n");
  } else {
    screen_shader->Use();
    glActiveTexture(GL_TEXTURE0);
    screen_shader->SetInt("screen_texture", 0);
  }

  fmt::print("Initialized OpenGL\n");
  return;
}

void r::Update() {
  if (!Engine::window) return;
  if (render_factor != prev_render_factor) {
    RecreateFramebuffer();
    prev_render_factor = render_factor;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glViewport(0, 0, Engine::width/render_factor, Engine::height/render_factor);
  glEnable(GL_DEPTH_TEST);
  if (directional_light)
    glClearColor(directional_light->ambient->x,
        directional_light->ambient->y,
        directional_light->ambient->z,
        1.0f);
  else
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void r::Render() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, Engine::width, Engine::height);
  glDisable(GL_DEPTH_TEST);
  dev::Update();
  if (dev::hud_enabled) {
    ImGui::Begin("Renderer Status");
    ImGui::Text("Directional light status: %d", (int)(directional_light != nullptr));
    ImGui::Text("Point light count: %d", (int)point_lights.size());
    ImGui::Text("Spot light count: %d", (int)spot_lights.size());
    ImGui::DragFloat3("Camera Position", glm::value_ptr(Engine::camera.position));
    ImGui::InputInt("Render Factor", &render_factor);
    if (render_factor <= 0) render_factor = 1;
    ImGui::SameLine();
    if (ImGui::Button("Set")) {
      RecreateFramebuffer();
    }
    ImGui::Text("Framebuffer (Colorbuffer)");
    ImGui::Image(colorbuffer, ImVec2(100, 100));
    ImGui::End();
  }
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  screen_shader->Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, colorbuffer);
  glBindVertexArray(screen_vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  dev::Render();
  glfwSwapBuffers(Engine::window);
}

void r::Quit() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteRenderbuffers(1, &renderbuffer);
  glDeleteFramebuffers(1, &framebuffer);
  if (Engine::window != nullptr) {
    glfwDestroyWindow(Engine::window);
    Engine::window = nullptr;
  }
  glfwTerminate();
  fmt::print("Terminated OpenGL\n");
}

glm::mat4 GetTransform(const glm::vec3 &pos, const glm::vec2 &scale, float rot) {
  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::rotate(transform, glm::radians(rot), glm::vec3(0.0, 0.0, 1.0));
  transform = glm::translate(transform, pos);
  transform = glm::scale(transform, glm::vec3(scale, 1));
  return transform;
}

void r::RenderTexture(const Material *material, const glm::vec3 &pos, const glm::vec2 &size, float rot) {
  if (!material) return;
  if (!material->IsValid()) return;

  glm::mat4 model = GetTransform(pos, size, rot),
    view = glm::translate(glm::mat4(1.0f), Engine::camera.position),
    projection = glm::perspective(glm::radians(60.0f), ((float)Engine::width / (float)Engine::height),
        0.1f, 100.0f);
  
  material->Bind();
  material->shader->SetMat4("model", model);
  material->shader->SetMat4("view", view);
  material->shader->SetVec3("viewPos", Engine::camera.position);
  material->shader->SetMat4("projection", projection);

  if (directional_light) directional_light->SetUniforms(material->shader);
  
  material->shader->SetInt("NUM_POINT_LIGHTS", (int)point_lights.size());
  for (int i = 0; i < point_lights.size(); i++) {
    if (point_lights[i]) point_lights[i]->SetUniforms(material->shader, i);
  }
  
  material->shader->SetInt("NUM_SPOT_LIGHTS", (int)spot_lights.size());
  for (int i = 0; i < spot_lights.size(); i++) {
    if (spot_lights[i]) spot_lights[i]->SetUniforms(material->shader, i);
  }

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RecreateFramebuffer() {
  if (renderbuffer != 0) {
    glDeleteRenderbuffers(1, &renderbuffer);
    renderbuffer = 0;
  }
  if (colorbuffer != 0) {
    glDeleteTextures(1, &colorbuffer);
    colorbuffer = 0;
  }
  if (framebuffer != 0) {
    glDeleteFramebuffers(1, &framebuffer);
    framebuffer = 0;
  }

  int framebuffer_width = Engine::width / render_factor;
  int framebuffer_height = Engine::height / render_factor;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glGenTextures(1, &colorbuffer);
  glBindTexture(GL_TEXTURE_2D, colorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
      framebuffer_width, framebuffer_height,
      0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorbuffer, 0);
  glGenRenderbuffers(1, &renderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
      framebuffer_width, framebuffer_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fmt::print("Failed to recreate framebuffer!\n");
    glDeleteRenderbuffers(1, &renderbuffer);
    glDeleteTextures(1, &colorbuffer);
    glDeleteFramebuffers(1, &framebuffer);
    renderbuffer = 0;
    colorbuffer = 0;
    framebuffer = 0;
  } else {
    fmt::print("Framebuffer recreated successfully ({}x{})\n", framebuffer_width, framebuffer_height);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
