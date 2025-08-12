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

#define TEXT_COLOR_RED ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define NUM_COLORBUFFERS 2
DirectionalLight* r::directional_light = nullptr;
std::vector<PointLight*> r::point_lights;
std::vector<SpotLight*> r::spot_lights;
unsigned int VAO, VBO, EBO;
unsigned int framebuffer, renderbuffer, colorbuffers[NUM_COLORBUFFERS];
unsigned int pingpong_framebuffer[2], pingpong_colorbuffer[2];
std::vector<unsigned int> attachments;
unsigned int screen_vao, screen_vbo;
Shader* screen_shader = nullptr, *screen_shader_blur;
float render_factor = 4, prev_render_factor;
float screen_exposure = 1.0;
int bloom_amount = 10;
bool bloom = true, antialias = true;

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
  glfwWindowHint(GLFW_SAMPLES, 4);
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
  glEnable(GL_MULTISAMPLE);
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
    screen_shader->SetInt("scene", 0);
    screen_shader->SetInt("blur", 1);
  }

  screen_shader_blur = f::LoadShader("screen_blur/vert.glsl", "screen_blur/frag.glsl");
  if (screen_shader_blur == nullptr) {
    fmt::print("Failed to load screen shader.\n");
  } else {
    screen_shader_blur->Use();
    screen_shader_blur->SetInt("image", 0);
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
  if (directional_light)
    glClearColor(directional_light->ambient->x,
        directional_light->ambient->y,
        directional_light->ambient->z,
        1.0f);
  else
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void r::Render() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //glViewport(0, 0, Engine::width, Engine::height);
  
  dev::Update();
  if (dev::hud_enabled) {
    ImGui::Begin("Renderer Status");
    ImGui::Text("Directional light status: %d", (int)(directional_light != nullptr));
    ImGui::Text("Point light count: %d", (int)point_lights.size());
    ImGui::Text("Spot light count: %d", (int)spot_lights.size());
    ImGui::DragFloat3("Camera Position", glm::value_ptr(Engine::camera.position));
    ImGui::SeparatorText("Screen Shader");
    ImGui::InputFloat("Exposure", &screen_exposure, 0.1f);
    ImGui::Checkbox("Anti-Alias", &antialias);
    ImGui::Checkbox("Bloom", &bloom);
    ImGui::InputInt("Bloom Amount", &bloom_amount);
  if (bloom_amount <= 0) bloom_amount = 1;
    ImGui::InputFloat("Render Factor", &render_factor, 0.5f);
    if (render_factor < 1) ImGui::TextColored(TEXT_COLOR_RED, "Downscaling");
    if (render_factor > 1) ImGui::TextColored(TEXT_COLOR_RED, "Upscaling");
    if (render_factor <= 0) render_factor = prev_render_factor;
    ImGui::Text("Colorbuffer");
    for (int i = 0; i < NUM_COLORBUFFERS; i++) {
      ImGui::Image(colorbuffers[i], ImVec2(100, 100));
    }
    ImGui::Text("Ping Pong Colorbuffer");
    for (int i = 0; i < NUM_COLORBUFFERS; i++) {
      ImGui::Image(pingpong_colorbuffer[i], ImVec2(100, 100));
    }
    ImGui::End();
  }

  bool horizontal = true, first_iteration = true;
  screen_shader_blur->Use();
  for (unsigned int i = 0; i < bloom_amount; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpong_framebuffer[horizontal]);
    screen_shader_blur->SetInt("horizontal", horizontal);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, first_iteration ? colorbuffers[1] : pingpong_colorbuffer[!horizontal]);
    glBindVertexArray(screen_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    horizontal = !horizontal;
    if (first_iteration) first_iteration = false;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if (antialias) glEnable(GL_MULTISAMPLE);
  else glDisable(GL_MULTISAMPLE);
  glViewport(0, 0, Engine::width, Engine::height);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  screen_shader->Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, colorbuffers[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, pingpong_colorbuffer[!horizontal]);
  screen_shader->SetInt("bloom", bloom);
  screen_shader->SetFloat("exposure", screen_exposure);
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
  if (!attachments.empty()) attachments.clear();
  if (renderbuffer != 0) {
    glDeleteRenderbuffers(1, &renderbuffer);
    renderbuffer = 0;
  }
  for (int i = 0; i < NUM_COLORBUFFERS; i++) {
    if (colorbuffers[i] != 0) {
      glDeleteTextures(1, &colorbuffers[i]);
      colorbuffers[i] = 0;
    }
  }
  if (framebuffer != 0) {
    glDeleteFramebuffers(1, &framebuffer);
    framebuffer = 0;
  }

  int framebuffer_width = Engine::width / render_factor;
  int framebuffer_height = Engine::height / render_factor;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); 
  glGenTextures(NUM_COLORBUFFERS, colorbuffers);
  for (unsigned int i = 0; i < NUM_COLORBUFFERS; i++) {
    glBindTexture(GL_TEXTURE_2D, colorbuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, framebuffer_width, framebuffer_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorbuffers[i], 0);
    attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
  }

  glGenRenderbuffers(1, &renderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
      framebuffer_width, framebuffer_height);
  
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
  glDrawBuffers(2, attachments.data());
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    fmt::print("Failed to recreate framebuffer!\n");
    glDeleteRenderbuffers(1, &renderbuffer);
    glDeleteFramebuffers(1, &framebuffer);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glGenFramebuffers(2, pingpong_framebuffer);
  glGenTextures(2, pingpong_colorbuffer);
  for (unsigned int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpong_framebuffer[i]);
    glBindTexture(GL_TEXTURE_2D, pingpong_colorbuffer[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, framebuffer_width, framebuffer_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpong_colorbuffer[i], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      fmt::print("Failed to recreate pingpong framebuffer!\n");
    }
  }
}
