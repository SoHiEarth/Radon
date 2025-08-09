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

int main(int argc, char** argv) {
#ifdef __APPLE__
  r::Init();
  i::Init();
#else
  SPAWN_THREAD(r::Init);
  SPAWN_THREAD(i::Init);
#endif
  SPAWN_THREAD(a::Init);
  SPAWN_THREAD(p::Init);
  dev::Init();

  for (auto& worker : workers) {
    worker.join();
  }
  workers.clear();

  if (Engine::window.load()) {
    Engine::current_level.store(new Level());
  }

  while (!glfwWindowShouldClose(Engine::window.load())) {
    i::Update();
    if (glfwGetKey(Engine::window.load(), GLFW_KEY_F1) == GLFW_PRESS) {
      dev::hud_enabled = !dev::hud_enabled;
    }

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

  dev::Quit();
  SPAWN_THREAD(a::Quit);
  SPAWN_THREAD(p::Quit);
#ifndef __APPLE__
  SPAWN_THREAD(i::Quit);
  SPAWN_THREAD(r::Quit);
#else
  i::Quit();
  r::Quit();
#endif
  for (auto& worker : workers) {
    worker.join();
  }
  workers.clear();
  return 0;
}
