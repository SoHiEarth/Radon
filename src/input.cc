#include <engine/input.h>
#include <GLFW/glfw3.h>

void i::Init() {
  // Do nothing, GLFW init is handled by rendering.
}

void i::Update() {
  glfwPollEvents();
}

void i::Quit() {
  // Do nothing, GLFW cleanup is handled by rendering.
}
