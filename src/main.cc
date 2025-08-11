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
  } \
  workers.clear();

int main(int argc, char** argv) {
  r::Init();
  i::Init();
  SPAWN_THREAD(a::Init);
  SPAWN_THREAD(p::Init);
  WAIT_WORKERS();
  dev::Init();

  if (!GET_LEVEL())
    Engine::level.store(new Level());

  i::AddHook({GLFW_KEY_ESCAPE, ButtonState::BUTTON_STATE_RELEASE}, []() {
    glfwSetWindowShouldClose(GET_WINDOW(), true);
  });

  i::AddHook({GLFW_KEY_F1, ButtonState::BUTTON_STATE_PRESS}, []() {
      dev::hud_enabled = !dev::hud_enabled;
  });

  i::AddHook({GLFW_KEY_W, ButtonState::BUTTON_STATE_HOLD}, []() {
      Engine::camera.position.y--;
  });

  i::AddHook({GLFW_KEY_S, ButtonState::BUTTON_STATE_HOLD}, []() {
      Engine::camera.position.y++;
  });

  i::AddHook({GLFW_KEY_A, ButtonState::BUTTON_STATE_HOLD}, []() {
      Engine::camera.position.x++;
  });

  i::AddHook({GLFW_KEY_D, ButtonState::BUTTON_STATE_HOLD}, []() {
      Engine::camera.position.x--;
  });

  while (!glfwWindowShouldClose(GET_WINDOW())) {
    dev::Update();
    r::Update();
    p::Update();
    if (GET_LEVEL()) GET_LEVEL()->Update();
    if (GET_LEVEL()) GET_LEVEL()->Render();
    dev::Render();
    r::Render();
    i::Update();
  }

  if (GET_LEVEL())
    GET_LEVEL()->Quit();

  dev::Quit();
  SPAWN_THREAD(a::Quit);
  SPAWN_THREAD(p::Quit);
  WAIT_WORKERS();
  i::Quit();
  r::Quit();
  return 0;
}
