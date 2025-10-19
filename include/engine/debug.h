#pragma once
#include <engine/interface.h>
#include <cstdint>
#include <functional>
#include <source_location>
#include <string_view>
#define CURRENT_LOCATION std::source_location::current()

struct DebugSettings {
  bool enable_trace_ = true;
  bool trace_source_file_ = false;
  bool trace_function_name_ = true;
  bool trace_line_number_ = false;
};

class IDebug : public Interface {
private:
  DebugSettings g_debug_settings_;
  static std::function<void(std::string_view, std::string_view, std::uint8_t)> callback_;

public:
  const char* Name() override {
    return "Debug";
  }
  DebugSettings& Settings() {
    return g_debug_settings_;
  }
  std::string GetTrace(const std::source_location& location);
  void SetCallback(std::function<void(std::string_view, std::string_view, std::uint8_t)> /*func*/) noexcept;
  void Log(std::string_view /*msg*/, const std::source_location& /*loc*/ = CURRENT_LOCATION) noexcept;
  void Warning(std::string_view /*msg*/, const std::source_location& /*loc*/ = CURRENT_LOCATION) noexcept;
  void Throw(std::string_view /*msg*/, const std::source_location& /*loc*/ = CURRENT_LOCATION);
};
