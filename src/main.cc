#include <engine/global.h>
#include <engine/render.h>
#include <engine/audio.h>
#include <engine/input.h>
#include <thread>
#include <fmt/core.h>

int main(int argc, char** argv) {
  fmt::print("Main Thread: Starting\n");
#ifndef __APPLE__
  std::thread r_init(r::Init);
#else
  r::Init();
#endif

  std::thread a_init(a::Init);
  a_init.join();

#ifndef __APPLE__
  r_init.join();
#endif

  while (!glfwWindowShouldClose(Engine::window.load())) {
    if (glfwGetKey(Engine::window.load(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      // Don't break the loop immediately, allow for cleanup
      glfwSetWindowShouldClose(Engine::window.load(), true);
    }
    r::Update();
    i::Update();
  }

#ifndef __APPLE__
  std::thread r_quit(r::Quit);
#endif
  std::thread a_quit(a::Quit);
  a_quit.join();

#ifndef __APPLE__
  r_quit.join();
#else
  r::Quit();
#endif

  fmt::print("Main Thread: Exiting\n");
  return 0;
}
