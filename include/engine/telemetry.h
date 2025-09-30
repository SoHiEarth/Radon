#pragma once
#include <engine/interface.h>
#include <chrono>
#include <map>
constexpr auto ENGINE_INIT_NAME = "Engine Init";
class ITelemetry : public Interface {
private:
  using Timing = std::map<const char*, std::chrono::milliseconds>;
  std::map<const char*, Timing> g_uploaded_timings;
protected:
  void i_Init() override;
  void i_Quit() override;

public:
  const char* name() override {
    return "Telemetry";
  }
  void BeginFrame();
  void BeginTimer(const char* /*name*/);
  void EndTimer(const char* /*name*/);
  void LogTimer(const char* /*name*/);
  void UploadTimings(const char* /*name*/, Timing& /*data*/);
  Timing& DownloadTimings(const char* /*name*/);
  Timing& GetTimings();
  Timing& GetLog();
};