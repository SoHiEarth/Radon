#include <engine/debug.h>
#include <fmt/core.h>
#include <format>
#include <utility>

std::function<void(std::string_view, std::string_view, std::uint8_t)> IDebug::callback_ =
    nullptr;

static std::string GetTrace(const std::source_location& location) {
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

void IDebug::SetCallback(
    std::function<void(std::string_view, std::string_view, std::uint8_t)> func) noexcept {
  callback_ = std::move(func);
}

void IDebug::Log(std::string_view msg, const std::source_location& loc) noexcept {
  fmt::print("{}: {}\n", GetTrace(loc), msg);
  if (callback_ != nullptr) {
    callback_(GetTrace(loc), msg, 0);
  }
}

void IDebug::Warning(std::string_view msg, const std::source_location& loc) noexcept {
  fmt::print("{}: {}\n", GetTrace(loc), msg);
  if (callback_ != nullptr) {
    callback_(GetTrace(loc), msg, 1);
  }
}

void IDebug::Throw(std::string_view msg, const std::source_location& loc) {
  if (callback_ != nullptr) {
    callback_(GetTrace(loc), msg, 2);
  }
  throw std::runtime_error(std::format("\t{}: {}\n", GetTrace(loc), msg));
}
