#include <engine/debug.h>
#include <engine/telemetry.h>
#include <utility>

std::map<const char*, std::chrono::high_resolution_clock::time_point> g_start_points{};
std::map<const char*, std::chrono::milliseconds> g_durations{};
std::map<const char*, std::chrono::milliseconds> g_duration_log{};

void ITelemetry::i_Init() {}

void ITelemetry::i_Quit() {
  g_durations.clear();
  g_start_points.clear();
}

void ITelemetry::BeginFrame() {
  if (!ITelemetry::GetStatus()) {
    return;
  }
  g_start_points.clear();
  g_durations.clear();
}

void ITelemetry::BeginTimer(const char* name) {
  if (!ITelemetry::GetStatus()) {
    return;
  }
  if (!g_start_points.contains(name)) {
    g_start_points.insert({name, std::chrono::high_resolution_clock::now()});
  } else {
    IDebug::Warning(std::format("Timer with name '{}' already exists!", name));
  }
}

void ITelemetry::EndTimer(const char* name) {
  if (!ITelemetry::GetStatus()) {
    return;
  }
  if (g_start_points.contains(name)) {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - g_start_points[name]);
    g_durations.insert({name, duration});
    g_start_points.erase(name);
  } else {
    IDebug::Warning(std::format("Timer with name '{}' does not exist!", name));
  }
}

void ITelemetry::LogTimer(const char* name) {
  if (!ITelemetry::GetStatus()) {
    return;
  }
  if (g_durations.contains(name)) {
    g_duration_log[name] = g_durations[name];
  } else {
    IDebug::Warning(std::format("No duration recorded for timer with name '{}'!", name));
  }
}

void ITelemetry::UploadTimings(const char* name, Timing& data) {
  if (!ITelemetry::GetStatus()) {
    return;
  }
  g_uploaded_timings[name] = std::move(data);
}

static std::map<const char*, std::chrono::milliseconds> g_empty = {};
std::map<const char*, std::chrono::milliseconds>& ITelemetry::DownloadTimings(const char* name) {
  if (!ITelemetry::GetStatus()) {
    return g_empty;
  }
  if (g_uploaded_timings.contains(name)) {
    return g_uploaded_timings[name];
  }
  IDebug::Throw(std::format("No uploaded timings found with name '{}'!", name));
  return g_empty;
}

std::map<const char*, std::chrono::milliseconds>& ITelemetry::GetTimings() {
  if (!ITelemetry::GetStatus()) {
    return g_empty;
  }
  return g_durations;
}

std::map<const char*, std::chrono::milliseconds>& ITelemetry::GetLog() {
  if (!ITelemetry::GetStatus()) {
    return g_empty;
  }
  return g_duration_log;
}
