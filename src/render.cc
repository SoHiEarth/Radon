#include <glad/glad.h>
#include <engine/render.h>
#include <engine/global.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>

void FBSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  Engine::width.store(width);
  Engine::height.store(height);
}

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
  
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fmt::print("Failed to initialize GLAD\n");
    glfwTerminate();
    return;
  }

  int width, height;
  glfwGetFramebufferSize(GET_WINDOW(), &width, &height);
  Engine::width.store(width);
  Engine::height.store(height);
  glfwSetFramebufferSizeCallback(GET_WINDOW(), FBSizeCallback);
  glViewport(0, 0, width, height);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  fmt::print("Initialized OpenGL\n");
  return;
}

void r::Update() {
  if (GET_WINDOW()) {
    glfwPollEvents();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}

void r::Present() {
  if (GET_WINDOW()) {
    glfwSwapBuffers(GET_WINDOW());
  }
}

void r::Quit() {
  if (GET_WINDOW()) {
    glfwDestroyWindow(GET_WINDOW());
    Engine::window.store(nullptr);
  }
  glfwTerminate();
  fmt::print("Terminated OpenGL\n");
}
