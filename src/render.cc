#include <glad/glad.h>
#include <engine/render.h>
#include <engine/global.h>
#include <classes/shader.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <engine/devgui.h>
#include <vector>
#include <classes/light.h>

DirectionalLight* directional_light;
std::vector<PointLight*> point_lights;
std::vector<SpotLight*> spot_lights;

void FBSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  Engine::width.store(width);
  Engine::height.store(height);
}

glm::vec3 light_ambient = {0.2f, 0.2f, 0.2f},
    light_diffuse = {0.5f, 0.5f, 0.5f},
    light_specular = {1.0f, 1.0f, 1.0f},
    light_position = {0.0f, 0.0f, 0.0f};

glm::vec3 pointLightPositions[] = {
	glm::vec3( 0.7f,  0.2f,  2.0f),
	glm::vec3( 2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3( 0.0f,  0.0f, -3.0f)
};

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
  Engine::window.store(glfwCreateWindow(800, 600, "Metal", nullptr, nullptr));
  if (!GET_WINDOW()) {
    fmt::print("Failed to create GLFW window\n");
    glfwTerminate();
    return;
  }
  glfwMakeContextCurrent(GET_WINDOW());
  Engine::width.store(800);
  Engine::height.store(600);
  glfwSetFramebufferSizeCallback(GET_WINDOW(), FBSizeCallback);
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fmt::print("Failed to initialize GLAD\n");
    glfwTerminate();
    return;
  }

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, Engine::width.load(), Engine::height.load());
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
  if (!GET_WINDOW()) return;
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
  glfwSwapBuffers(GET_WINDOW());
}

void r::Quit() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  if (GET_WINDOW()) {
    glfwDestroyWindow(GET_WINDOW());
    Engine::window.store(nullptr);
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

void r::RenderTexture(const Material* material, const glm::vec3 &pos, const glm::vec2 &size, float rot) {
  if (!material) return;
  if (!material->IsValid()) return;
  Shader* shader = material->shader; // For readability

  // Transformations
  glm::mat4 model = GetTransform(pos, size, rot),
    view = glm::translate(glm::mat4(1.0f), Engine::camera.position),
    projection = glm::perspective(glm::radians(60.0f),
        ((float)Engine::width.load() / (float)Engine::height.load()),
        0.1f, 100.0f);
  material->Bind();
  shader->SetMat4("model", model);
  shader->SetMat4("view", view);
  shader->SetMat4("projection", projection);

  if (directional_light)
    directional_light->SetUniforms(material->shader);
  shader->SetInt("NUM_POINT_LIGHTS", (int)point_lights.size());
  for (int i = 0; i < point_lights.size(); i++) {
    if (point_lights[i])
      point_lights[i]->SetUniforms(material->shader, i);
  }
  shader->SetInt("NUM_SPOT_LIGHTS", (int)spot_lights.size());
  for (int i = 0; i < spot_lights.size(); i++) {
    if (spot_lights[i])
      spot_lights[i]->SetUniforms(material->shader, i);
  }

  shader->SetVec3("viewPos", Engine::camera.position);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void r::AddDirectionalLight(DirectionalLight *light) {
  if (!light) return;
  RemoveDirectionalLight(directional_light);
  light->active = true;
  directional_light = light;
}

void r::AddPointLight(PointLight *light) {
  if (!light) return;
  light->active = true;
  point_lights.push_back(light);
}

void r::AddSpotLight(SpotLight *light) {
  if (!light) return;
  light->active = true;
  spot_lights.push_back(light);
}

void r::RemoveDirectionalLight(DirectionalLight *target) {
  if (!target) return;
  target->active = false;
  directional_light = nullptr;
}

void r::RemovePointLight(PointLight *target) {
  if (!target) return;
  for (int i = 0; i < point_lights.size(); i++) {
    if (point_lights[i] == target) {
      target->active = false;
      point_lights.erase(point_lights.begin() + i);
    }
  }
}

void r::RemoveSpotLight(SpotLight *target) {
  if (!target) return;
  for (int i = 0; i < spot_lights.size(); i++) {
    if (spot_lights[i] == target) {
      target->active = false;
      spot_lights.erase(spot_lights.begin() + i);
    }
  }
}
