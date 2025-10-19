#pragma once
#include <engine/interface.h>
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
  static void SetCallback(
      std::function<void(std::string_view, std::string_view, std::uint8_t)> /*func*/) noexcept;
  static void Log(std::string_view /*msg*/,
                  const std::source_location& /*loc*/ = std::source_location::current()) noexcept;
  static void Warning(std::string_view /*msg*/, const std::source_location& /*loc*/ =
                                               std::source_location::current()) noexcept;
  static void Throw(std::string_view /*msg*/,
                    const std::source_location& /*loc*/ = std::source_location::current());
};
