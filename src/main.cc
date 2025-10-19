#include <GLFW/glfw3.h>
#include <classes/camera.h>
#include <classes/level.h>
#include <engine/asset_manager.h>
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
#include "engine/engine.h"

int main(int argc, char** argv) {
  Engine engine;
  try {
    engine.Init();
  } catch (std::exception& e) {
    engine.GetDebug().Log(std::format("Initialization failure: {}", e.what()));
    tinyfd_messageBox("Initialization Failure", std::string(e.what()).c_str(), "ok", "error", 1);
    return -1;
  } catch (...) {
    engine.GetDebug().Log("Unknown initialization failure");
    tinyfd_messageBox("Initialization Failure", "Unknown failure", "ok", "error", 1);
    return -1;
  }

  engine.GetInput().AddHook({GLFW_KEY_ESCAPE, ButtonState::kRelease}, [](Engine* engine) {
    glfwSetWindowShouldClose(engine->GetRenderer().GetWindow(), true);
  });
  engine.GetInput().AddHook({GLFW_KEY_F1, ButtonState::kPress}, [](Engine* engine) {
    engine->GetGui().SetHud(!engine->GetGui().GetHud());
  });

  try {
    while (glfwWindowShouldClose(engine.GetRenderer().GetWindow()) == 0) {
      engine.Update();
      engine.Render();
    }
  } catch (std::exception& e) {
    engine.GetDebug().Log(std::format("Runtime failure: {}", e.what()));
    tinyfd_messageBox("Runtime Failure", std::string(e.what()).c_str(), "ok", "error", 1);
  }

  try {
    engine.Quit();
  } catch (std::exception& e) {
    engine.GetDebug().Log(std::format("Quit failure: {}", e.what()));
    tinyfd_messageBox("Quit Failure", e.what(), "ok", "error", 1);
  }
  return 0;
}
