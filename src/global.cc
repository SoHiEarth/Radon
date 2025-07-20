#include <engine/global.h>
#include <atomic>
#include <GLFW/glfw3.h>

std::atomic<GLFWwindow*> Engine::window{nullptr};
