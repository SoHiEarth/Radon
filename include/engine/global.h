#ifndef ENGINE_GLOBAL_H
#define ENGINE_GLOBAL_H

#include <atomic>
#include <GLFW/glfw3.h>
#include <classes/level.h>
#include <classes/camera.h>

#define _INT_ATOMIC std::atomic<int>
#define _WINDOW_ATOMIC std::atomic<GLFWwindow*>
#define _LEVEL_ATOMIC std::atomic<Level*>
#define GET_WINDOW() Engine::window.load()
#define GET_LEVEL() Engine::level.load()

namespace Engine {
  extern _WINDOW_ATOMIC window;
  extern _INT_ATOMIC width;
  extern _INT_ATOMIC height;
  extern _LEVEL_ATOMIC level;
  extern Camera camera;
}

#endif // ENGINE_GLOBAL_H
