#ifndef ENGINE_GLOBAL_H
#define ENGINE_GLOBAL_H

#include <atomic>
#include <GLFW/glfw3.h>

namespace Engine {
  extern std::atomic<GLFWwindow*> window;
  extern std::atomic<int> width;
  extern std::atomic<int> height;
}

#endif // ENGINE_GLOBAL_H
