#pragma once
#include <engine/interface.h>
#include <chrono>
#include <map>
constexpr auto kEngineInitName = "Engine Init";
class ITelemetry : public Interface {
private:
  using Timing = std::map<const char*, std::chrono::milliseconds>;
  std::map<const char*, Timing> g_uploaded_timings_;

protected:
  void IInit() override;
  void IQuit() override;

public:
  const char* Name() override {
    return "Telemetry";
  }
  static void BeginFrame();
  static void BeginTimer(const char* /*name*/);
  static void EndTimer(const char* /*name*/);
  static void LogTimer(const char* /*name*/);
  void UploadTimings(const char* /*name*/, Timing& /*data*/);
  Timing& DownloadTimings(const char* /*name*/);
  static Timing& GetTimings();
  static Timing& GetLog();
};