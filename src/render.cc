#include <glad/glad.h>
#include <engine/render.h>
#include <engine/global.h>
#include <engine/devgui.h>
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


void FBSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  Engine::width = width;
  Engine::height = height;
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
unsigned int VAO, VBO, EBO;

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
  fmt::print("Initialized OpenGL\n");
  return;
}

void r::Update() {
  if (!Engine::window) return;
  if (directional_light)
    glClearColor(directional_light->ambient.x,
        directional_light->ambient.y,
        directional_light->ambient.z,
        1.0f);
  else
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (dev::hud_enabled) {
    ImGui::Begin("Renderer Status");
    ImGui::Text("Directional light status: %d", (int)(directional_light != nullptr));
    ImGui::Text("Point light count: %d", (int)point_lights.size());
    ImGui::Text("Spot light count: %d", (int)spot_lights.size());
    ImGui::DragFloat3("Camera Position", glm::value_ptr(Engine::camera.position));
    ImGui::End();
  }
}

void r::Render() {
  glfwSwapBuffers(Engine::window);
}

void r::Quit() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
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
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
