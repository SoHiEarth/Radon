#ifndef DEBUG_H
#define DEBUG_H
#include <cstdint>
#include <functional>
#include <source_location>
#include <string_view>

struct DebugSettings {
  bool enable_trace_ = true;
  bool trace_source_file_ = false;
  bool trace_function_name_ = true;
  bool trace_line_number_ = false;
};

namespace debug {
extern DebugSettings g_debug_settings;
void g_set_callback(std::function<void(const char*, const char*, std::uint8_t)> func) noexcept;
void Log(const char* msg,
         const std::source_location& location = std::source_location::current()) noexcept;
void Log(std::string_view msg,
         const std::source_location& location = std::source_location::current()) noexcept;
void Warning(const char* msg,
             const std::source_location& location = std::source_location::current()) noexcept;
void Warning(std::string_view msg,
             const std::source_location& location = std::source_location::current()) noexcept;
void Throw(const char* msg, const std::source_location& location = std::source_location::current());
void Throw(std::string_view msg,
           const std::source_location& location = std::source_location::current());
}  // namespace debug
#endif  // DEBUG_H
