#include <engine/debug.h>
#include <fmt/core.h>
#include <format>
#include <utility>
DebugSettings debug::g_debug_settings{};

std::string GetTrace(const std::source_location& location) {
  std::string trace;
  if (debug::g_debug_settings.trace_source_file_) {
    trace += location.file_name();
}
  if (debug::g_debug_settings.trace_source_file_) {
    trace += "(" + std::to_string(location.line()) + ":" + std::to_string(location.column()) + ")";
}
  if (debug::g_debug_settings.trace_function_name_) {
    trace += location.function_name();
}
  return trace;
}

std::function<void(const char*, const char*, std::uint8_t)> g_debug_callback = nullptr;
void debug::SetCallback(std::function<void(const char*, const char*, std::uint8_t)> func) noexcept {
  g_debug_callback = std::move(func);
  debug::Log("Set debug callback");
}

void debug::Log(const char* msg, const std::source_location& location) noexcept {
  fmt::print("{}: {}\n", GetTrace(location), msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(location).c_str(), msg, 0);
  }
}

void debug::Log(std::string_view msg, const std::source_location& location) noexcept {
  fmt::print("{}: {}\n", GetTrace(location), msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(location).c_str(), msg.data(), 0);
  }
}

void debug::Warning(std::string_view msg, const std::source_location& location) noexcept {
  fmt::print("{}: {}\n", GetTrace(location), msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(location).c_str(), msg.data(), 1);
  }
}

void debug::Warning(const char* msg, const std::source_location& location) noexcept {
  fmt::print("{}: {}\n", GetTrace(location), msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(location).c_str(), msg, 1);
  }
}

void debug::Throw(std::string_view msg, const std::source_location& location) {
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(location).c_str(), msg.data(), 2);
  }
  throw std::runtime_error(std::format("\t{}: {}\n", GetTrace(location), msg));
}

void debug::Throw(const char* msg, const std::source_location& location) {
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(location).c_str(), msg, 2);
  }
  throw std::runtime_error(std::format("\t{}: {}\n", GetTrace(location), msg));
}
