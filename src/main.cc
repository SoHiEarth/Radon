#include <GLFW/glfw3.h>
#include <classes/camera.h>
#include <classes/level.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <engine/devgui.h>
#include <engine/filesystem.h>
#include <engine/input.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <format>
#include <filesystem>
#include <tinyfiledialogs/tinyfiledialogs.h>

int main(int argc, char** argv) {
  try {
    filesystem::Init();
    render::Init();
    dev::Init();
    input::Init();
    audio::Init();
    physics::Init();
  } catch (std::exception& e) {
    debug::Log(GET_TRACE, std::format("Initialization failure: {}", e.what()));
    tinyfd_messageBox("Initialization Failure", e.what(), "ok", "error", 1);
    return -1;
  }

  input::AddHook({GLFW_KEY_ESCAPE, ButtonState::kRelease},
                 []() { glfwSetWindowShouldClose(render::g_window, true); });
  input::AddHook({GLFW_KEY_F1, ButtonState::kPress},
                 []() { dev::g_hud_enabled = !dev::g_hud_enabled; });

  try {
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
  } catch (std::exception& e) {
    debug::Log(GET_TRACE, std::format("Runtime failure: {}", e.what()));
    tinyfd_messageBox("Runtime Failure", e.what(), "ok", "error", 1);
  }

  if (filesystem::g_level != nullptr) {
    filesystem::g_level->Quit();
  }

  try {
    dev::Quit();
    audio::Quit();
    physics::Quit();
    input::Quit();
    render::Quit();
  } catch (std::exception& e) {
    debug::Log(GET_TRACE, std::format("Quit failure: {}", e.what()));
    tinyfd_messageBox("Quit Failure", e.what(), "ok", "error", 1);
  }
  return 0;
}
