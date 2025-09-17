#include <engine/debug.h>
#include <fmt/core.h>
#include <format>
#include <utility>
#define DEBUG_IMPL_TRACE_FILE trace.at(0).source_file()
#define DEBUG_IMPL_TRACE_FUNCTION trace.at(0).description()
#define DEBUG_IMPL_TRACE_LINE trace.at(0).source_line()
#define DEBUG_IMPL_TRACE                                                                          \
  (debug::g_debug_settings.trace_source_file_ ? DEBUG_IMPL_TRACE_FILE : "") +                     \
      (debug::g_debug_settings.trace_function_name_                                               \
           ? ("::" + std::string(DEBUG_IMPL_TRACE_FUNCTION))                                      \
           : "") +                                                                                \
      (debug::g_debug_settings.trace_line_number_ ? (":" + std::to_string(DEBUG_IMPL_TRACE_LINE)) \
                                                  : "")
DebugSettings debug::g_debug_settings{};

#ifndef NDEBUG
#ifdef MDEBUG_DISABLE_TRACE
std::function<void(const char*, std::uint8_t)> g_debug_callback = nullptr;
void debug::SetCallback(
    std::function<void(const char*, std::uint8_t)> callback) noexcept {
  g_debug_callback = std::move(callback);
  debug::Log(GET_TRACE, "Set debug callback");
}

void debug::Log(int /*unused*/, const char* fmt) noexcept {
  auto msg = std::format("Log: {}\n", fmt);
  fmt::print("{}", msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(msg.c_str(), 0);
  }
}

void debug::Log(int /*unused*/, std::string_view fmt) noexcept {
  auto msg = std::format("Log: {}\n", fmt);
  fmt::print("{}", msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(msg.c_str(), 0);
  }
}

void debug::Warning(int /*unused*/, const char* fmt) noexcept {
  auto msg = std::format("Warning: {}\n", fmt);
  if (g_debug_callback != nullptr) {
    g_debug_callback(msg.c_str(), 1);
  }
}

void debug::Warning(int /*unused*/, std::string_view fmt) noexcept {
  auto msg = std::format("Warning: {}\n", fmt);
  if (g_debug_callback != nullptr) {
    g_debug_callback(msg.c_str(), 1);
  }
}

void debug::Throw(int /*unused*/, const char* fmt) {
  auto msg = std::format("\tException: {}\n", fmt);
  if (g_debug_callback != nullptr) {
    g_debug_callback(msg.c_str(), 2);
  }
  throw std::runtime_error(msg);
}

void debug::Throw(int /*unused*/, std::string_view fmt) {
  auto msg = std::format("\tException: {}\n", fmt);
  if (g_debug_callback != nullptr) {
    g_debug_callback(msg.c_str(), 2);
  }
  throw std::runtime_error(msg);
}
#else
std::function<void(const char*, const char*, std::uint8_t)> debug_callback = nullptr;
void debug::SetCallback(std::function<void(const char*, const char*, std::uint8_t)> func) noexcept {
  debug_callback = func;
  debug::Log(GET_TRACE, "Set debug callback");
}

void debug::Log(const std::stacktrace trace, std::string_view fmt) noexcept {
  fmt::print("{}: {}\n", DEBUG_IMPL_TRACE, fmt);
  if (debug_callback != nullptr) {
    debug_callback((DEBUG_IMPL_TRACE).c_str(), fmt.data(), 0);
  }
}

void debug::Log(const std::stacktrace trace, const char* fmt) noexcept {
  fmt::print("{}: {}\n", DEBUG_IMPL_TRACE, fmt);
  if (debug_callback != nullptr) {
    debug_callback((DEBUG_IMPL_TRACE).c_str(), fmt, 0);
  }
}

void debug::Warning(const std::stacktrace trace, std::string_view fmt) noexcept {
  fmt::print("{}: {}\n", DEBUG_IMPL_TRACE, fmt);
  if (debug_callback != nullptr) {
    debug_callback((DEBUG_IMPL_TRACE).c_str(), fmt.data(), 1);
  }
}

void debug::Warning(const std::stacktrace trace, const char* fmt) noexcept {
  fmt::print("{}: {}\n", DEBUG_IMPL_TRACE, fmt);
  if (debug_callback != nullptr) {
    debug_callback((DEBUG_IMPL_TRACE).c_str(), fmt, 1);
  }
}

void debug::Throw(const std::stacktrace trace, std::string_view fmt) {
  if (debug_callback != nullptr) {
    debug_callback((DEBUG_IMPL_TRACE).c_str(), fmt.data(), 2);
  }
  throw std::runtime_error(std::format("\t{}: {}\n", DEBUG_IMPL_TRACE, fmt));
}

void debug::Throw(const std::stacktrace trace, const char* fmt) {
  if (debug_callback != nullptr) {
    debug_callback((DEBUG_IMPL_TRACE).c_str(), fmt, 2);
  }
  throw std::runtime_error(std::format("\t{}: {}\n", DEBUG_IMPL_TRACE, fmt));
}
#endif
#else
void debug::Log(int /*unused*/, const char* msg) noexcept {
  if (debug_callback != nullptr) {
    debug_callback(msg, 0);
  }
}

void debug::Log(int /*unused*/, std::string_view msg) noexcept {
  if (debug_callback != nullptr) {
    debug_callback(msg.data(), 0);
  }
}

void debug::Warning(int /*unused*/, const char* msg) noexcept {
  if (debug_callback != nullptr) {
    debug_callback(msg, 1);
  }
}

void debug::Warning(int /*unused*/, std::string_view msg) noexcept {
  if (debug_callback != nullptr) {
    debug_callback(msg.data(), 1);
  }
}

void debug::Throw(int /*unused*/, const char* fmt) {
  auto msg = std::format("\tException: {}\n", fmt);
  if (debug_callback != nullptr) {
    debug_callback(msg.c_str(), 2);
  }
  throw std::runtime_error(msg);
}

void debug::Throw(int /*unused*/, std::string_view fmt) {
  auto msg = std::format("\tException: {}\n", fmt);
  if (debug_callback != nullptr) {
    debug_callback(msg.c_str(), 2);
  }
  throw std::runtime_error(msg);
}
#endif
