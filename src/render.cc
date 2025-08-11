#include <glad/glad.h>
#include <engine/render.h>
#include <engine/global.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

void FBSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  Engine::width.store(width);
  Engine::height.store(height);
}

float vertices[] = {
    // positions
     0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
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

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fmt::print("Failed to initialize GLAD\n");
    glfwTerminate();
    return;
  }
  
  glViewport(0, 0, Engine::width.load(), Engine::height.load());
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  fmt::print("Initialized OpenGL\n");
  return;
}

void r::Update() {
  if (GET_WINDOW()) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }
}

void r::Present() {
  if (GET_WINDOW()) {
    glfwSwapBuffers(GET_WINDOW());
  }
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
  transform = glm::translate(transform, pos);
  transform = glm::rotate(transform, glm::radians(rot), glm::vec3(0.0, 0.0, 1.0));
  transform = glm::scale(transform, glm::vec3(scale, 1));
  return transform;
}

void r::RenderTexture(const Texture *texture, const Shader *shader, const glm::vec3 &pos, const glm::vec2 &size, float rot) {
  if (!texture || !shader) return;
  glUseProgram(shader->id);
  glm::mat4 transform = GetTransform(pos, size, rot),
    projection = glm::perspective(glm::radians(60.0f),
        (float)(Engine::width.load() / Engine::height.load()),
        0.1f, 100.0f);
  shader->SetMat4("projection", projection);
  shader->SetMat4("transform", transform);
  glBindTexture(GL_TEXTURE_2D, texture->id);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
