#include <engine/debug.h>
#include <fmt/core.h>
#include <format>
#include <utility>

std::string GetTrace(const std::source_location& location) {
  std::string trace;
  if (IDebug::Get<IDebug>().Settings().trace_source_file_) {
    trace += location.file_name();
  }
  if (IDebug::Get<IDebug>().Settings().trace_source_file_) {
    trace += "(" + std::to_string(location.line()) + ":" + std::to_string(location.column()) + ")";
  }
  if (IDebug::Get<IDebug>().Settings().trace_function_name_) {
    trace += location.function_name();
  }
  return trace;
}

std::function<void(const char*, const char*, std::uint8_t)> g_debug_callback = nullptr;
void IDebug::SetCallback(
    std::function<void(const char*, const char*, std::uint8_t)> func) noexcept {
  g_debug_callback = std::move(func);
  IDebug::Log("Set debug callback");
}

void IDebug::Log(const char* msg, const location& loc) noexcept {
  fmt::print("{}: {}\n", GetTrace(loc), msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(loc).c_str(), msg, 0);
  }
}

void IDebug::Log(std::string_view msg, const location& loc) noexcept {
  fmt::print("{}: {}\n", GetTrace(loc), msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(loc).c_str(), msg.data(), 0);
  }
}

void IDebug::Warning(std::string_view msg, const location& loc) noexcept {
  fmt::print("{}: {}\n", GetTrace(loc), msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(loc).c_str(), msg.data(), 1);
  }
}

void IDebug::Warning(const char* msg, const location& loc) noexcept {
  fmt::print("{}: {}\n", GetTrace(loc), msg);
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(loc).c_str(), msg, 1);
  }
}

void IDebug::Throw(std::string_view msg, const location& loc) {
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(loc).c_str(), msg.data(), 2);
  }
  throw std::runtime_error(std::format("\t{}: {}\n", GetTrace(loc), msg));
}

void IDebug::Throw(const char* msg, const location& loc) {
  if (g_debug_callback != nullptr) {
    g_debug_callback(GetTrace(loc).c_str(), msg, 2);
  }
  throw std::runtime_error(std::format("\t{}: {}\n", GetTrace(loc), msg));
}
