#include <engine/debug.h>
#include <engine/telemetry.h>
#include <utility>
#include "engine/engine.h"

std::map<std::string, std::chrono::high_resolution_clock::time_point> g_start_points{};
std::map<std::string, std::chrono::milliseconds> g_durations{};
std::map<std::string, std::chrono::milliseconds> g_duration_log{};
static std::map<std::string, std::chrono::milliseconds> g_empty = {};

void ITelemetry::IInit() {}

void ITelemetry::IQuit() {
  g_durations.clear();
  g_start_points.clear();
}

void ITelemetry::BeginFrame() {
  if (GetState() != InterfaceState::kInitialized) {
    return;
  }
  g_start_points.clear();
  g_durations.clear();
}

void ITelemetry::BeginTimer(const std::string& name) {
  if (GetState() != InterfaceState::kInitialized) {
    return;
  }
  if (!g_start_points.contains(name)) {
    g_start_points.insert({name, std::chrono::high_resolution_clock::now()});
  } else {
    engine_->GetDebug().Warning(std::format("Timer with name '{}' already exists!", name));
  }
}

void ITelemetry::EndTimer(const std::string& name) {
  if (GetState() != InterfaceState::kInitialized) {
    return;
  }
  if (g_start_points.contains(name)) {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - g_start_points[name]);
    g_durations.insert({name, duration});
    g_start_points.erase(name);
  } else {
    engine_->GetDebug().Warning(std::format("Timer with name '{}' does not exist!", name));
  }
}

void ITelemetry::LogTimer(const std::string& name) {
  if (GetState() != InterfaceState::kInitialized) {
    return;
  }
  if (g_durations.contains(name)) {
    g_duration_log[name] = g_durations[name];
  } else {
    engine_->GetDebug().Warning(
        std::format("No duration recorded for timer with name '{}'!", name));
  }
}

void ITelemetry::UploadTimings(const std::string& name, Timing& data) {
  if (GetState() != InterfaceState::kInitialized) {
    return;
  }
  g_uploaded_timings_[name] = std::move(data);
}

std::map<std::string, std::chrono::milliseconds>& ITelemetry::DownloadTimings(
    const std::string& name) {
  if (GetState() != InterfaceState::kInitialized) {
    return g_empty;
  }
  if (g_uploaded_timings_.contains(name)) {
    return g_uploaded_timings_[name];
  }
  engine_->GetDebug().Throw(std::format("No uploaded timings found with name '{}'!", name));
  return g_empty;
}

std::map<std::string, std::chrono::milliseconds>& ITelemetry::GetTimings() {
  if (GetState() != InterfaceState::kInitialized) {
    return g_empty;
  }
  return g_durations;
}

std::map<std::string, std::chrono::milliseconds>& ITelemetry::GetLog() {
  if (GetState() != InterfaceState::kInitialized) {
    return g_empty;
  }
  return g_duration_log;
}
