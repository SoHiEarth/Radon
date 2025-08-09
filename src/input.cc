#include <engine/input.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>

void i::Init() {
  glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  fmt::print("Initialized Input\n");
}

void i::Update() {
  glfwPollEvents();
}

void i::Quit() {
  glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetCursorPos(glfwGetCurrentContext(), 0.0, 0.0);
  fmt::print("Terminated Input\n");
}
