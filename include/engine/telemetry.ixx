module;
#include <chrono>
#include <map>
export module metal.telemetry;

export namespace telemetry {
void Init();
void BeginFrame();
void BeginTimer(const char* name);
void EndTimer(const char* name);
void LogTimer(const char* name);
void UploadTimings(const char* name, std::map<std::string, std::chrono::milliseconds> data);
std::map<std::string, std::chrono::milliseconds> DownloadTimings(const char* name);
std::map<std::string, std::chrono::milliseconds> GetTimings();
std::map<std::string, std::chrono::milliseconds> GetLog();
void Quit();
}  // namespace telemetry