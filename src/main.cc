#include <GLFW/glfw3.h>
#include <classes/camera.h>
#include <classes/level.h>
#include <engine/audio.h>
#include <engine/devgui.h>
#include <engine/global.h>
#include <engine/input.h>
#include <engine/physics.h>
#include <engine/filesystem.h>
#include <engine/render.h>
#include <fmt/core.h>
#include <thread>
#include <vector>
#define CAMERA_SPEED 0.1
std::vector<std::thread> workers;

#define SPAWN_THREAD(func)                        \
  {                                               \
    std::thread thread(func);                     \
    workers.push_back(std::move(thread));         \
    fmt::print("Spawned thread for {}\n", #func); \
  }

#define WAIT_WORKERS()           \
  for (auto& worker : workers) { \
    worker.join();               \
  }                              \
  workers.clear();

int main(int argc, char** argv) {
  r::Init();
  i::Init();
  SPAWN_THREAD(a_Init);
  SPAWN_THREAD(p_Init);
  WAIT_WORKERS();
  dev::Init();

  Engine::g_level = f::LoadLevel("test.xml");
  /*
  if (Engine::g_level == nullptr) {
    Engine::g_level = new Level();
  }
  */
  i::AddHook({GLFW_KEY_ESCAPE, ButtonState::kRelease},
             []() { glfwSetWindowShouldClose(Engine::g_window, true); });

  i::AddHook({GLFW_KEY_F1, ButtonState::kPress},
             []() { dev::g_hud_enabled = !dev::g_hud_enabled; });

  i::AddHook({GLFW_KEY_W, ButtonState::kHold},
             []() { Engine::g_camera.position_.y += CAMERA_SPEED; });

  i::AddHook({GLFW_KEY_S, ButtonState::kHold},
             []() { Engine::g_camera.position_.y -= CAMERA_SPEED; });

  i::AddHook({GLFW_KEY_A, ButtonState::kHold},
             []() { Engine::g_camera.position_.x -= CAMERA_SPEED; });

  i::AddHook({GLFW_KEY_D, ButtonState::kHold},
             []() { Engine::g_camera.position_.x += CAMERA_SPEED; });

  while (glfwWindowShouldClose(Engine::g_window) == 0) {
    i::Update();
    if (Engine::g_level != nullptr) {
      Engine::g_level->Update();
    }
    r::Update();
    p_Update();
    if (Engine::g_level != nullptr) {
      Engine::g_level->Render();
    }
    r::Render();
  }

  if (Engine::g_level != nullptr) {
    Engine::g_level->Quit();
  }

  dev::Quit();
  SPAWN_THREAD(a_Quit);
  SPAWN_THREAD(p_Quit);
  WAIT_WORKERS();
  i::Quit();
  r::Quit();
  return 0;
}
