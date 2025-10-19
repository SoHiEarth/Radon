#pragma once
#include <engine/interface.h>
#include <chrono>
#include <map>
constexpr auto kEngineInitName = "Engine Init";
class ITelemetry : public Interface {
private:
  using Timing = std::map<std::string, std::chrono::milliseconds>;
  std::map<std::string, Timing> g_uploaded_timings_;

protected:
  void IInit() override;
  void IQuit() override;

public:
  const char* Name() override {
    return "Telemetry";
  }
  static void BeginFrame();
  static void BeginTimer(const std::string& /*name*/);
  static void EndTimer(const std::string& /*name*/);
  static void LogTimer(const std::string& /*name*/);
  void UploadTimings(const std::string& /*name*/, Timing& /*data*/);
  Timing& DownloadTimings(const std::string& /*name*/);
  static Timing& GetTimings();
  static Timing& GetLog();
};