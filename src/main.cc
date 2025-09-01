#include <GLFW/glfw3.h>
#include <classes/camera.h>
#include <classes/level.h>
#include <engine/audio.h>
#include <engine/devgui.h>
#include <engine/filesystem.h>
#include <engine/input.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <fmt/core.h>
#define CAMERA_SPEED 0.1

int main(int argc, char** argv) {
  render::Init();
  input::Init();
  audio::Init();
  physics::Init();
  dev::Init();

  try {
    filesystem::g_level = filesystem::LoadLevel("test.xml");
  } catch (std::exception& e) {
    fmt::print("Caught exception, falling back to new level. Details: {}\n", e.what());
    if (filesystem::g_level == nullptr) {
      filesystem::g_level = new Level();
    }
  }

  input::AddHook({GLFW_KEY_ESCAPE, ButtonState::kRelease},
                 []() { glfwSetWindowShouldClose(render::g_window, true); });

  input::AddHook({GLFW_KEY_F1, ButtonState::kPress},
                 []() { dev::g_hud_enabled = !dev::g_hud_enabled; });

  input::AddHook({GLFW_KEY_W, ButtonState::kHold},
                 []() { render::g_camera.position_.y += CAMERA_SPEED; });

  input::AddHook({GLFW_KEY_S, ButtonState::kHold},
                 []() { render::g_camera.position_.y -= CAMERA_SPEED; });

  input::AddHook({GLFW_KEY_A, ButtonState::kHold},
                 []() { render::g_camera.position_.x -= CAMERA_SPEED; });

  input::AddHook({GLFW_KEY_D, ButtonState::kHold},
                 []() { render::g_camera.position_.x += CAMERA_SPEED; });

  while (glfwWindowShouldClose(render::g_window) == 0) {
    input::Update();
    if (filesystem::g_level != nullptr) {
      filesystem::g_level->Update();
    }
    render::Update();
    physics::Update();
    if (filesystem::g_level != nullptr) {
      filesystem::g_level->Render();
    }
    render::Render();
  }

  if (filesystem::g_level != nullptr) {
    filesystem::g_level->Quit();
  }

  dev::Quit();
  audio::Quit();
  physics::Quit();
  input::Quit();
  render::Quit();
  return 0;
}
