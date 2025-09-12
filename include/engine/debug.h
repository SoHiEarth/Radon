#ifndef DEBUG_H
#define DEBUG_H
#include <string_view>
#ifdef MDEBUG_DISABLE_TRACE
#define GET_TRACE 0x0
#else
#include <stacktrace>
#define GET_TRACE std::stacktrace::current()
#endif

struct DebugSettings {
  bool enable_trace = true;
  bool trace_source_file = true;
  bool trace_function_name = false;
  bool trace_line_number = false;
};

namespace debug {
extern DebugSettings g_debug_settings;
#ifdef MDEBUG_DISABLE_TRACE
void Log(int, const char* fmt);
void Log(int, std::string_view fmt);
void Throw(int, const char* fmt);
void Throw(int, std::string_view fmt);
#else
void Log(const std::stacktrace trace, const char* fmt);
void Log(const std::stacktrace trace, std::string_view fmt);
void Throw(const std::stacktrace trace, const char* fmt);
void Throw(const std::stacktrace trace, std::string_view fmt);
#endif
}  // namespace debug
#endif  // DEBUG_H
