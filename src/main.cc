#include <GLFW/glfw3.h>
#include <classes/camera.h>
#include <classes/level.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <engine/devgui.h>
#include <engine/input.h>
#include <engine/io.h>
#include <engine/physics.h>
#include <engine/localization.h>
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
constexpr const char* kTimerLocalizationInitName = "Localization Init";
constexpr const char* kTimerPhysicsInitName = "Physics Init";

constexpr const char* kTimerInputUpdateName = "Input Update";
constexpr const char* kTimerLevelUpdateName = "Level Update";
constexpr const char* kTimerRenderUpdateName = "Render Update";
constexpr const char* kTimerPhysicsUpdateName = "Physics Update";
constexpr const char* kTimerLevelRenderName = "Level Render";
constexpr const char* kTimerRenderRenderName = "Render Render";
constexpr const char* kTimerAudioUpdateName = "Audio Update";

#define TIME(FUNC, NAME)       \
  ITelemetry::Get<ITelemetry>().BeginTimer(NAME); \
  FUNC;                        \
  ITelemetry::Get<ITelemetry>().EndTimer(NAME);   \
  ITelemetry::Get<ITelemetry>().LogTimer(NAME);

int main(int argc, char** argv) {
  try {
    ITelemetry::Get<ITelemetry>().Start();
    ITelemetry::Get<ITelemetry>().BeginTimer(kTimerInitName);
    TIME(IIO::Get<IIO>().Start(), kTimerIoInitName);
    TIME(IRenderer::Get<IRenderer>().Start(), kTimerRenderInitName);
    TIME(IGui::Get<IGui>().Start(), kTimerDevguiInitName);
    TIME(IInput::Get<IInput>().Start(), kTimerInputInitName);
    TIME(IAudio::Get<IAudio>().Start(), kTimerAudioInitName);
    TIME(ILocalization::Get<ILocalization>().Start(), kTimerLocalizationInitName);
    TIME(IPhysics::Get<IPhysics>().Start(), kTimerPhysicsInitName);
    ITelemetry::Get<ITelemetry>().EndTimer(kTimerInitName);
    ITelemetry::Get<ITelemetry>().LogTimer(kTimerInitName);
    auto& timings = ITelemetry::Get<ITelemetry>().GetTimings();
    for (const auto& [name, duration] : timings) {
      IDebug::Log(std::format("Telemetry: {}: {}ms", name, duration.count()));
    }
    ITelemetry::Get<ITelemetry>().UploadTimings(ENGINE_INIT_NAME, timings);

  } catch (std::exception& e) {
    IDebug::Log(std::format("Initialization failure: {}", e.what()));
    tinyfd_messageBox("Initialization Failure", e.what(), "ok", "error", 1);
    return -1;
  }

  IInput::Get<IInput>().AddHook({GLFW_KEY_ESCAPE, ButtonState::kRelease}, []() {
    glfwSetWindowShouldClose(IRenderer::Get<IRenderer>().GetWindow(), true);
  });
  IInput::Get<IInput>().AddHook({GLFW_KEY_F1, ButtonState::kPress},
                                []() { IGui::Get<IGui>().SetHud(!IGui::Get<IGui>().GetHud()); });

  try {
    while (glfwWindowShouldClose(IRenderer::Get<IRenderer>().GetWindow()) == 0) {
      ITelemetry::Get<ITelemetry>().BeginFrame();
      TIME(IInput::Get<IInput>().Update(), kTimerInputUpdateName);
      if (IIO::Get<IIO>().GetLevel() != nullptr) {
        TIME(IIO::Get<IIO>().GetLevel()->Update(), kTimerLevelUpdateName);
      }
      TIME(IPhysics::Get<IPhysics>().Update(), kTimerPhysicsUpdateName);
      TIME(IRenderer::Get<IRenderer>().Update(), kTimerRenderUpdateName);
      if (IIO::Get<IIO>().GetLevel() != nullptr) {
        TIME(IIO::Get<IIO>().GetLevel()->Render(), kTimerLevelRenderName);
      }
      TIME(IRenderer::Get<IRenderer>().Render(), kTimerRenderRenderName);
      TIME(IAudio::Get<IAudio>().Update(), kTimerAudioUpdateName);
    }
  } catch (std::exception& e) {
    IDebug::Log(std::format("Runtime failure: {}", e.what()));
    tinyfd_messageBox("Runtime Failure", e.what(), "ok", "error", 1);
  }

  if (IIO::Get<IIO>().GetLevel() != nullptr) {
    IIO::Get<IIO>().GetLevel()->Quit();
  }

  try {
    IGui::Get<IGui>().Stop();
    ILocalization::Get<ILocalization>().Stop();
    IAudio::Get<IAudio>().Stop();
    IPhysics::Get<IPhysics>().Stop();
    IInput::Get<IInput>().Stop();
    IRenderer::Get<IRenderer>().Stop();
  } catch (std::exception& e) {
    IDebug::Log(std::format("Quit failure: {}", e.what()));
    tinyfd_messageBox("Quit Failure", e.what(), "ok", "error", 1);
  }
  return 0;
}
