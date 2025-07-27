#include <engine/global.h>
#include <atomic>
#include <GLFW/glfw3.h>

std::atomic<GLFWwindow*> Engine::window{nullptr};
std::atomic<int> Engine::width{800};
std::atomic<int> Engine::height{600};
