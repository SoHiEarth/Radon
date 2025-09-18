import metal.debug;
#include <source_location>
#include <string>
#include <functional>
#include <format>
#include <fmt/core.h>

const std::string GetTrace(const std::source_location& location) {
  if (!debug::enable_trace_)
    return "";
  std::string trace;
  if (debug::trace_source_file_)
    trace += location.file_name();
  if (debug::trace_source_file_)
    trace += "(" + std::to_string(location.line()) + ":" + std::to_string(location.column()) + ")";
  if (debug::trace_function_name_)
    trace += location.function_name();
  return trace;
}

std::function<void(const char*, const char*, std::uint8_t)> debug_callback = nullptr;

namespace debug {
bool enable_trace_ = true;
bool trace_source_file_ = false;
bool trace_function_name_ = true;
bool trace_line_number_ = false;

void SetCallback(std::function<void(const char*, const char*, std::uint8_t)> func) noexcept {
  debug_callback = std::move(func);
  debug::Log("Set debug callback", std::source_location::current());
}

void Log(const char* msg, const std::source_location& location) noexcept {
  fmt::print("{}: {}\n", GetTrace(location), msg);
  if (debug_callback != nullptr) {
    debug_callback(GetTrace(location).c_str(), msg, 0);
  }
}

void Log(std::string_view msg, const std::source_location& location) noexcept {
  fmt::print("{}: {}\n", GetTrace(location), msg);
  if (debug_callback != nullptr) {
    debug_callback(GetTrace(location).c_str(), msg.data(), 0);
  }
}

void Warning(std::string_view msg, const std::source_location& location) noexcept {
  fmt::print("{}: {}\n", GetTrace(location), msg);
  if (debug_callback != nullptr) {
    debug_callback(GetTrace(location).c_str(), msg.data(), 1);
  }
}

void Warning(const char* msg, const std::source_location& location) noexcept {
  fmt::print("{}: {}\n", GetTrace(location), msg);
  if (debug_callback != nullptr) {
    debug_callback(GetTrace(location).c_str(), msg, 1);
  }
}

void Throw(std::string_view msg, const std::source_location& location) {
  if (debug_callback != nullptr) {
    debug_callback(GetTrace(location).c_str(), msg.data(), 2);
  }
  throw std::runtime_error(std::format("\t{}: {}\n", GetTrace(location), msg));
}

void Throw(const char* msg, const std::source_location& location) {
  if (debug_callback != nullptr) {
    debug_callback(GetTrace(location).c_str(), msg, 2);
  }
  throw std::runtime_error(std::format("\t{}: {}\n", GetTrace(location), msg));
}
}  // namespace debug