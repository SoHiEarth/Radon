#include <GLFW/glfw3.h>
#include <classes/camera.h>
#include <classes/level.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <engine/devgui.h>
#include <engine/input.h>
#include <engine/io.h>
#include <engine/localization.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <engine/telemetry.h>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <format>
constexpr const char* kTimerInitName = "Engine Init";
constexpr const char* kTimerIoInitName = "IO Init";
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

#define TIME(FUNC, NAME)                          \
  Interface::Get<ITelemetry>().BeginTimer(NAME); \
  FUNC;                                           \
  Interface::Get<ITelemetry>().EndTimer(NAME);   \
  Interface::Get<ITelemetry>().LogTimer(NAME);

int main(int argc, char** argv) {
  try {
    Interface::Get<ITelemetry>().Start();
    Interface::Get<ITelemetry>().BeginTimer(kTimerInitName);
    TIME(Interface::Get<IIO>().Start(), kTimerIoInitName);
    TIME(Interface::Get<IRenderer>().Start(), kTimerRenderInitName);
    TIME(Interface::Get<IGui>().Start(), kTimerDevguiInitName);
    TIME(Interface::Get<IInput>().Start(), kTimerInputInitName);
    TIME(Interface::Get<IAudio>().Start(), kTimerAudioInitName);
    TIME(Interface::Get<ILocalization>().Start(), kTimerLocalizationInitName);
    TIME(Interface::Get<IPhysics>().Start(), kTimerPhysicsInitName);
    Interface::Get<ITelemetry>().EndTimer(kTimerInitName);
    Interface::Get<ITelemetry>().LogTimer(kTimerInitName);
    Interface::Get<ITelemetry>().UploadTimings(ENGINE_INIT_NAME,
                                                Interface::Get<ITelemetry>().GetTimings());
  } catch (std::exception& e) {
    IDebug::Log(std::format("Initialization failure: {}", e.what()));
    tinyfd_messageBox("Initialization Failure", std::string(e.what()).c_str(), "ok", "error", 1);
    return -1;
  } catch (...) {
    IDebug::Log("Unknown initialization failure");
    tinyfd_messageBox("Initialization Failure", "Unknown failure", "ok", "error", 1);
    return -1;
  }

  Interface::Get<IInput>().AddHook({GLFW_KEY_ESCAPE, ButtonState::kRelease}, []() {
    glfwSetWindowShouldClose(Interface::Get<IRenderer>().GetWindow(), true);
  });
  Interface::Get<IInput>().AddHook({GLFW_KEY_F1, ButtonState::kPress},
                                []() { Interface::Get<IGui>().SetHud(!Interface::Get<IGui>().GetHud()); });

  try {
    while (glfwWindowShouldClose(Interface::Get<IRenderer>().GetWindow()) == 0) {
      Interface::Get<ITelemetry>().BeginFrame();
      TIME(Interface::Get<IInput>().Update(), kTimerInputUpdateName);
      if (Interface::Get<IIO>().GetLevel() != nullptr) {
        TIME(Interface::Get<IIO>().GetLevel()->Update(), kTimerLevelUpdateName);
      }
      TIME(Interface::Get<IPhysics>().Update(), kTimerPhysicsUpdateName);
      TIME(Interface::Get<IRenderer>().Update(), kTimerRenderUpdateName);
      if (IIO::Get<IIO>().GetLevel() != nullptr) {
        TIME(IIO::Get<IIO>().GetLevel()->Render(), kTimerLevelRenderName);
      }
      TIME(IRenderer::Get<IRenderer>().Render(), kTimerRenderRenderName);
      TIME(IAudio::Get<IAudio>().Update(), kTimerAudioUpdateName);
    }
  } catch (std::exception& e) {
    IDebug::Log(std::format("Runtime failure: {}", e.what()));
    tinyfd_messageBox("Runtime Failure", std::string(e.what()).c_str(), "ok", "error", 1);
  }

  if (IIO::Get<IIO>().GetLevel() != nullptr) {
    IIO::Get<IIO>().GetLevel()->Quit();
  }

  try {
    IPhysics::Get<IPhysics>().Stop();
    ILocalization::Get<ILocalization>().Stop();
    IAudio::Get<IAudio>().Stop();
    IInput::Get<IInput>().Stop();
    IGui::Get<IGui>().Stop();
    IRenderer::Get<IRenderer>().Stop();
    IIO::Get<IIO>().Stop();
    Interface::Get<ITelemetry>().Stop();
  } catch (std::exception& e) {
    IDebug::Log(std::format("Quit failure: {}", e.what()));
    tinyfd_messageBox("Quit Failure", e.what(), "ok", "error", 1);
  }
  return 0;
}
