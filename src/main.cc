#include <engine/global.h>
#include <engine/render.h>
#include <engine/audio.h>
#include <engine/input.h>
#include <engine/physics.h>
#include <engine/filesystem.h>
#include <engine/devgui.h>
#include <thread>
#include <vector>
#include <fmt/core.h>

std::vector<std::thread> workers;

#define SPAWN_THREAD(func) \
  { \
    std::thread t(func); \
    workers.push_back(std::move(t)); \
    fmt::print("Spawned thread for {}\n", #func); \
  }

#define WAIT_WORKERS() \
  for (auto& worker : workers) { \
    worker.join(); \
  }

int main(int argc, char** argv) {
  r::Init();
  i::Init();
  SPAWN_THREAD(a::Init);
  SPAWN_THREAD(p::Init);
  dev::Init();

  WAIT_WORKERS();
  workers.clear();

  if (Engine::window.load()) {
    Engine::current_level.store(new Level());
  }

  i::AddHook({GLFW_KEY_ESCAPE, ButtonState::BUTTON_STATE_RELEASE}, []() {
    glfwSetWindowShouldClose(Engine::window.load(), true);
  });

  i::AddHook({GLFW_KEY_F1, ButtonState::BUTTON_STATE_PRESS}, []() {
      dev::hud_enabled = !dev::hud_enabled;
  });

  while (!glfwWindowShouldClose(Engine::window.load())) {
    i::Update();
    dev::Update();
    r::Update();
    if (Engine::current_level.load())
      Engine::current_level.load()->Update();
    dev::LateUpdate();
    r::Present();
  }

  if (Engine::current_level.load()) {
    Engine::current_level.load()->Quit();
  }

  SPAWN_THREAD(a::Quit);
  SPAWN_THREAD(p::Quit);
  WAIT_WORKERS();
  workers.clear();
  i::Quit();
  dev::Quit();
  r::Quit();
  return 0;
}
