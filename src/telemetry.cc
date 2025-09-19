#include <engine/debug.h>
#include <engine/telemetry.h>

#include <utility>

std::map<std::string, std::chrono::high_resolution_clock::time_point> g_start_points;
std::map<std::string, std::chrono::milliseconds> g_durations;
std::map<std::string, std::chrono::milliseconds> g_duration_log;

void telemetry::Init() {
  debug::Log("Initialized telemetry");
}

void telemetry::BeginFrame() {
  g_start_points.clear();
  g_durations.clear();
}

void telemetry::BeginTimer(const char* name) {
  if (!g_start_points.contains(name)) {
    g_start_points.insert({name, std::chrono::high_resolution_clock::now()});
  } else {
    debug::Warning(std::format("Timer with name '{}' already exists!", name));
  }
}

void telemetry::EndTimer(const char* name) {
  if (g_start_points.contains(name)) {
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end_time - g_start_points[name]);
    g_durations.insert({name, duration});
    g_start_points.erase(name);
  } else {
    debug::Warning(std::format("Timer with name '{}' does not exist!", name));
  }
}

void telemetry::LogTimer(const char* name) {
  if (g_durations.contains(name)) {
    g_duration_log[name] = g_durations[name];
  } else {
    debug::Warning(std::format("No duration recorded for timer with name '{}'!", name));
  }
}

std::map<std::string, std::map<std::string, std::chrono::milliseconds>> g_uploaded_timings;
void telemetry::UploadTimings(const char* name,
                              std::map<std::string, std::chrono::milliseconds> data) {
  g_uploaded_timings[name] = std::move(data);
}

std::map<std::string, std::chrono::milliseconds> telemetry::DownloadTimings(const char* name) {
  if (g_uploaded_timings.contains(name)) {
    return g_uploaded_timings[name];
  }     debug::Warning(std::format("No uploaded timings found with name '{}'!", name));
    return {};
 
}

std::map<std::string, std::chrono::milliseconds> telemetry::GetTimings() {
  return g_durations;
}

std::map<std::string, std::chrono::milliseconds> telemetry::GetLog() {
  return g_duration_log;
}

void telemetry::Quit() {
  g_durations.clear();
  g_start_points.clear();
  debug::Log("Quit telemetry");
}
