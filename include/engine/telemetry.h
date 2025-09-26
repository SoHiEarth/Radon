#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <chrono>
#include <map>
#define ENGINE_INIT_NAME "Engine Init"

namespace telemetry {
void Init();
void BeginFrame();
void BeginTimer(const char*);
void EndTimer(const char*);
void LogTimer(const char*);
void UploadTimings(const char*, std::map<std::string, std::chrono::milliseconds>);
std::map<std::string, std::chrono::milliseconds> DownloadTimings(const char*);
std::map<std::string, std::chrono::milliseconds> GetTimings();
std::map<std::string, std::chrono::milliseconds> GetLog();
void Quit();
}  // namespace telemetry

#endif  // TELEMETRY_H