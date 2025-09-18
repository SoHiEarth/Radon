#include <GLFW/glfw3.h>
#include <classes/camera.h>
#include <classes/level.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <engine/devgui.h>
#include <engine/input.h>
#include <engine/io.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <engine/telemetry.h>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <format>
#define TIMER_INIT_NAME "Engine Init"
#define TIMER_IO_INIT_NAME "I/O Init"
#define TIMER_RENDER_INIT_NAME "Render Init"
#define TIMER_DEVGUI_INIT_NAME "Dev GUI Init"
#define TIMER_INPUT_INIT_NAME "Input Init"
#define TIMER_AUDIO_INIT_NAME "Audio Init"
#define TIMER_PHYSICS_INIT_NAME "Physics Init"

#define TIMER_INPUT_UPDATE_NAME "Input Update"
#define TIMER_LEVEL_UPDATE_NAME "Level Update"
#define TIMER_RENDER_UPDATE_NAME "Render Update"
#define TIMER_PHYSICS_UPDATE_NAME "Physics Update"
#define TIMER_LEVEL_RENDER_NAME "Level Render"
#define TIMER_RENDER_RENDER_NAME "Render Render"
#define TIMER_AUDIO_UPDATE_NAME "Audio Update"

#define TIME(FUNC, NAME)       \
  telemetry::BeginTimer(NAME); \
  FUNC;                        \
  telemetry::EndTimer(NAME);   \
  telemetry::LogTimer(NAME);

int main(int argc, char** argv) {
  try {
    telemetry::Init();
    telemetry::BeginTimer(TIMER_INIT_NAME);
    TIME(io::Init(), TIMER_IO_INIT_NAME);
    TIME(render::Init(), TIMER_RENDER_INIT_NAME);
    TIME(dev::Init(), TIMER_DEVGUI_INIT_NAME);
    TIME(input::Init(), TIMER_INPUT_INIT_NAME);
    TIME(audio::Init(), TIMER_AUDIO_INIT_NAME);
    TIME(physics::Init(), TIMER_PHYSICS_INIT_NAME);
    telemetry::EndTimer(TIMER_INIT_NAME);
    telemetry::LogTimer(TIMER_INIT_NAME);
    auto timings = telemetry::GetTimings();
    for (const auto& [name, duration] : timings) {
      debug::Log(std::format("Telemetry: {}: {}ms", name, duration.count()));
    }
    telemetry::UploadTimings(ENGINE_INIT_NAME, timings);

  } catch (std::exception& e) {
    debug::Log(std::format("Initialization failure: {}", e.what()));
    tinyfd_messageBox("Initialization Failure", e.what(), "ok", "error", 1);
    return -1;
  }

  input::AddHook({GLFW_KEY_ESCAPE, ButtonState::kRelease},
                 []() { glfwSetWindowShouldClose(render::g_window, true); });
  input::AddHook({GLFW_KEY_F1, ButtonState::kPress},
                 []() { dev::g_hud_enabled = !dev::g_hud_enabled; });

  try {
    while (glfwWindowShouldClose(render::g_window) == 0) {
      telemetry::BeginFrame();
      TIME(input::Update(), TIMER_INPUT_UPDATE_NAME);
      if (io::g_level != nullptr) {
        TIME(io::g_level->Update(), TIMER_LEVEL_UPDATE_NAME);
      }
      TIME(physics::Update(), TIMER_PHYSICS_UPDATE_NAME);
      TIME(render::Update(), TIMER_RENDER_UPDATE_NAME);
      if (io::g_level != nullptr) {
        TIME(io::g_level->Render(), TIMER_LEVEL_RENDER_NAME);
      }
      TIME(render::Render(), TIMER_RENDER_RENDER_NAME);
      TIME(audio::Update(), TIMER_AUDIO_UPDATE_NAME);
    }
  } catch (std::exception& e) {
    debug::Log(std::format("Runtime failure: {}", e.what()));
    tinyfd_messageBox("Runtime Failure", e.what(), "ok", "error", 1);
  }

  if (io::g_level != nullptr) {
    io::g_level->Quit();
  }

  try {
    dev::Quit();
    audio::Quit();
    physics::Quit();
    input::Quit();
    render::Quit();
  } catch (std::exception& e) {
    debug::Log(std::format("Quit failure: {}", e.what()));
    tinyfd_messageBox("Quit Failure", e.what(), "ok", "error", 1);
  }
  return 0;
}
