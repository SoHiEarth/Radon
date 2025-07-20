#include <GLFW/glfw3.h>
#include <engine/global.h>

int main(int argc, char** argv) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  Engine::window.store(glfwCreateWindow(800, 600, "Metal", nullptr, nullptr));
  if (!Engine::window.load()) {
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(Engine::window.load());
  while (!glfwWindowShouldClose(Engine::window.load())) {
    glfwSwapBuffers(Engine::window.load());
    glfwPollEvents();
  }

  glfwDestroyWindow(Engine::window.load());
  glfwTerminate();
  return 0;
}
