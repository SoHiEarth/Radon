#pragma once
#include <engine/interface.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <engine/devgui.h>
#include <engine/input.h>
#include <engine/io.h>
#include <engine/localization.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <engine/telemetry.h>

class Engine {
public:
  IAudio& GetAudio() {
    return Interface::Get<IAudio>();
  }
  IDebug& GetDebug() {
    return Interface::Get<IDebug>();
  }
  IGui& GetGui() {
    return Interface::Get<IGui>();
  }
  IInput& GetInput() {
    return Interface::Get<IInput>();
  }
  IIO& GetIO() {
    return Interface::Get<IIO>();
  }
  ILocalization& GetLocalization() {
    return Interface::Get<ILocalization>();
  }
  IPhysics& GetPhysics() {
    return Interface::Get<IPhysics>();
  }
  IRenderer& GetRenderer() {
    return Interface::Get<IRenderer>();
  }
  ITelemetry& GetTelemetry() {
    return Interface::Get<ITelemetry>();
  }
};