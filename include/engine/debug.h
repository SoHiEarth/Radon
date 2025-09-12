#ifndef DEBUG_H
#define DEBUG_H
#include <stacktrace>
#define GET_TRACE std::stacktrace::current()
struct DebugSettings {
  bool enable_trace = true;
  bool trace_source_file = true;
  bool trace_function_name = false;
  bool trace_line_number = false;
};

namespace debug {
extern DebugSettings g_debug_settings;
void Log(std::stacktrace trace, const char* fmt);
void Log(std::stacktrace trace, std::string_view fmt);
void Throw(std::stacktrace trace, const char* fmt);
void Throw(std::stacktrace trace, std::string_view fmt);
}  // namespace debug
#endif  // DEBUG_H