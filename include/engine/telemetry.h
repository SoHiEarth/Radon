#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <map>
#include <chrono>

namespace telemetry {
void Init();
void BeginFrame();
void BeginTimer(const char* name);
void EndTimer(const char* name);
void LogTimer(const char* name);
std::map<std::string, std::chrono::milliseconds> GetTimings();
std::map<std::string, std::chrono::milliseconds> GetLog();
void Quit();
}

#endif  // TELEMETRY_H