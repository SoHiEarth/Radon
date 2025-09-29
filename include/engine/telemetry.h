#pragma once
#include <engine/interface.h>
#include <chrono>
#include <map>
constexpr auto ENGINE_INIT_NAME = "Engine Init";

class ITelemetry : public Interface {
  using timing = std::map<std::string, std::chrono::milliseconds>;
public:
  const char* name() override {
    return "Telemetry";
  }
  void Init() override;
  void Quit() override;

  void BeginFrame();
  void BeginTimer(const char* /*name*/);
  void EndTimer(const char* /*name*/);
  void LogTimer(const char* /*name*/);
  void UploadTimings(const char* /*name*/, timing& /*data*/);
  timing& DownloadTimings(const char* /*name*/);
  timing& GetTimings();
  timing& GetLog();
};