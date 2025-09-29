#include <engine/debug.h>
#include <engine/telemetry.h>

#include <utility>

std::map<std::string, std::chrono::high_resolution_clock::time_point> g_start_points;
std::map<std::string, std::chrono::milliseconds> g_durations;
std::map<std::string, std::chrono::milliseconds> g_duration_log;

void ITelemetry::Init() {
}

void ITelemetry::BeginFrame() {
  g_start_points.clear();
  g_durations.clear();
}

void ITelemetry::BeginTimer(const char* name) {
  if (!g_start_points.contains(name)) {
    g_start_points.insert({name, std::chrono::high_resolution_clock::now()});
  } else {
    IDebug::Warning(std::format("Timer with name '{}' already exists!", name));
  }
}

void ITelemetry::EndTimer(const char* name) {
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
  if (g_durations.contains(name)) {
    g_duration_log[name] = g_durations[name];
  } else {
    IDebug::Warning(std::format("No duration recorded for timer with name '{}'!", name));
  }
}

std::map<std::string, std::map<std::string, std::chrono::milliseconds>> g_uploaded_timings;
void ITelemetry::UploadTimings(const char* name, timing& data) {
  g_uploaded_timings[name] = std::move(data);
}

std::map<std::string, std::chrono::milliseconds>& ITelemetry::DownloadTimings(const char* name) {
  if (g_uploaded_timings.contains(name)) {
    return g_uploaded_timings[name];
  }
  IDebug::Warning(std::format("No uploaded timings found with name '{}'!", name));
  static std::map<std::string, std::chrono::milliseconds> empty_map;
  return empty_map;
}

std::map<std::string, std::chrono::milliseconds>& ITelemetry::GetTimings() {
  return g_durations;
}

std::map<std::string, std::chrono::milliseconds>& ITelemetry::GetLog() {
  return g_duration_log;
}

void ITelemetry::Quit() {
  g_durations.clear();
  g_start_points.clear();
}
