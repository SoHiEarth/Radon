#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <chrono>
#include <map>
#define ENGINE_INIT_NAME "Engine Init"

namespace telemetry {
void Init();
void BeginFrame();
void BeginTimer(const char* /*name*/);
void EndTimer(const char* /*name*/);
void LogTimer(const char* /*name*/);
void UploadTimings(const char* /*name*/, std::map<std::string, std::chrono::milliseconds> /*data*/);
std::map<std::string, std::chrono::milliseconds> DownloadTimings(const char* /*name*/);
std::map<std::string, std::chrono::milliseconds> GetTimings();
std::map<std::string, std::chrono::milliseconds> GetLog();
void Quit();
}  // namespace telemetry

#endif  // TELEMETRY_H