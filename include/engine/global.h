#ifndef ENGINE_GLOBAL_H
#define ENGINE_GLOBAL_H

#include <atomic>
#include <GLFW/glfw3.h>
#include <classes/level.h>

#define _INT_ATOMIC std::atomic<int>
#define _WINDOW_ATOMIC std::atomic<GLFWwindow*>
#define _LEVEL_ATOMIC std::atomic<Level*>
#define GET_WINDOW() Engine::window.load()
#define SET_WINDOW(window) Engine::window.store(window)

namespace Engine {
  extern _WINDOW_ATOMIC window;
  extern _INT_ATOMIC width;
  extern _INT_ATOMIC height;
  extern _LEVEL_ATOMIC current_level;
}

#endif // ENGINE_GLOBAL_H
