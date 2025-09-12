#include <engine/debug.h>
#include <fmt/core.h>
#include <format>
#define DEBUG_IMPL_TRACE_FILE trace.at(0).source_file()
#define DEBUG_IMPL_TRACE_FUNCTION trace.at(0).description()
#define DEBUG_IMPL_TRACE_LINE trace.at(0).source_line()
#define DEBUG_IMPL_TRACE                                                                         \
  (debug::g_debug_settings.trace_source_file ? DEBUG_IMPL_TRACE_FILE : "") +                     \
      (debug::g_debug_settings.trace_function_name                                               \
           ? ("::" + std::string(DEBUG_IMPL_TRACE_FUNCTION))                                     \
           : "") +                                                                               \
      (debug::g_debug_settings.trace_line_number ? (":" + std::to_string(DEBUG_IMPL_TRACE_LINE)) \
                                                 : "")
DebugSettings debug::g_debug_settings{};

#ifdef MDEBUG_DISABLE_TRACE
void debug::Log(int /*unused*/, const char* fmt) {
  fmt::print("Log: {}\n", fmt);
}

void debug::Log(int /*unused*/, std::string_view fmt) {
  fmt::print("Log: {}\n", fmt);
}

void debug::Throw(int /*unused*/, const char* fmt) {
  throw std::runtime_error(std::format("\tException: {}\n", fmt));
}

void debug::Throw(int /*unused*/, std::string_view fmt) {
  throw std::runtime_error(std::format("\tException: {}\n", fmt));
}
#else
void debug::Log(const std::stacktrace trace, std::string_view fmt) {
  fmt::print("{}: {}\n", DEBUG_IMPL_TRACE, fmt);
}

void debug::Log(const std::stacktrace trace, const char* fmt) {
  fmt::print("{}: {}\n", DEBUG_IMPL_TRACE, fmt);
}

void debug::Throw(const std::stacktrace trace, std::string_view fmt) {
  throw std::runtime_error(std::format("\t{}: {}\n", DEBUG_IMPL_TRACE, fmt));
}

void debug::Throw(const std::stacktrace trace, const char* fmt) {
  throw std::runtime_error(std::format("\t{}: {}\n", DEBUG_IMPL_TRACE, fmt));
}
#endif
