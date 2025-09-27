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
constexpr const char* kTimerInitName = "Engine Init";
constexpr const char* kTimerIoInitName = "I/O Init";
constexpr const char* kTimerRenderInitName = "Render Init";
constexpr const char* kTimerDevguiInitName = "Dev GUI Init";
constexpr const char* kTimerInputInitName = "Input Init";
constexpr const char* kTimerAudioInitName = "Audio Init";
constexpr const char* kTimerPhysicsInitName = "Physics Init";

constexpr const char* kTimerInputUpdateName = "Input Update";
constexpr const char* kTimerLevelUpdateName = "Level Update";
constexpr const char* kTimerRenderUpdateName = "Render Update";
constexpr const char* kTimerPhysicsUpdateName = "Physics Update";
constexpr const char* kTimerLevelRenderName = "Level Render";
constexpr const char* kTimerRenderRenderName = "Render Render";
constexpr const char* kTimerAudioUpdateName = "Audio Update";

#define TIME(FUNC, NAME)       \
  telemetry::BeginTimer(NAME); \
  FUNC;                        \
  telemetry::EndTimer(NAME);   \
  telemetry::LogTimer(NAME);

int main(int argc, char** argv) {
  try {
    telemetry::Init();
    telemetry::BeginTimer(kTimerInitName);
    TIME(io::Init(), kTimerIoInitName);
    TIME(render::Init(), kTimerRenderInitName);
    TIME(dev::Init(), kTimerDevguiInitName);
    TIME(input::Init(), kTimerInputInitName);
    TIME(audio::Init(), kTimerAudioInitName);
    TIME(physics::Init(), kTimerPhysicsInitName);
    telemetry::EndTimer(kTimerInitName);
    telemetry::LogTimer(kTimerInitName);
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
      TIME(input::Update(), kTimerInputUpdateName);
      if (io::g_level != nullptr) {
        TIME(io::g_level->Update(), kTimerLevelUpdateName);
      }
      TIME(physics::Update(), kTimerPhysicsUpdateName);
      TIME(render::Update(), kTimerRenderUpdateName);
      if (io::g_level != nullptr) {
        TIME(io::g_level->Render(), kTimerLevelRenderName);
      }
      TIME(render::Render(), kTimerRenderRenderName);
      TIME(audio::Update(), kTimerAudioUpdateName);
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
