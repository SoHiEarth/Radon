#ifndef ENGINE_GLOBAL_H
#define ENGINE_GLOBAL_H

#include <atomic>
#include <GLFW/glfw3.h>

namespace Engine {
  extern std::atomic<GLFWwindow*> window;
}

#endif // ENGINE_GLOBAL_H
