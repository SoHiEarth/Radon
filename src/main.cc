#include <engine/global.h>
#include <engine/render.h>
#include <engine/audio.h>
#include <engine/input.h>
#include <thread>
#include <fmt/core.h>

int main(int argc, char** argv) {
  fmt::print("Main Thread: Starting\n");
  std::thread r_init(r::Init);
  std::thread a_init(a::Init);
  r_init.join();
  a_init.join();

  while (!glfwWindowShouldClose(Engine::window.load())) {
    if (glfwGetKey(Engine::window.load(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      // Don't break the loop immediately, allow for cleanup
      glfwSetWindowShouldClose(Engine::window.load(), true);
    }
    r::Update();
    i::Update();
  }

  std::thread r_quit(r::Quit);
  std::thread a_quit(a::Quit);
  r_quit.join();
  a_quit.join();
  fmt::print("Main Thread: Exiting\n");
  return 0;
}
