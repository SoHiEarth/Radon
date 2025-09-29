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
  DebugSettings g_debug_settings;
  static std::function<void(const char*, const char*, std::uint8_t)> callback;

protected:
  const char* name() override {
    return "Debug";
  }
public:
  DebugSettings& Settings() {
    return g_debug_settings;
  }
  void SetCallback(std::function<void(const char*, const char*, std::uint8_t)> /*func*/) noexcept;
  static void Log(const char* /*msg*/,
    const std::source_location& /*loc*/ = std::source_location::current()) noexcept;
  static void Log(std::string_view /*msg*/,
    const std::source_location& /*loc*/ = std::source_location::current()) noexcept;
  static void Warning(const char* /*msg*/,
    const std::source_location& /*loc*/ = std::source_location::current()) noexcept;
  static void Warning(std::string_view /*msg*/,
    const std::source_location& /*loc*/ = std::source_location::current()) noexcept;
  static void Throw(const char* /*msg*/,
    const std::source_location& /*loc*/ = std::source_location::current());
  static void Throw(std::string_view /*msg*/,
    const std::source_location& /*loc*/ = std::source_location::current());
};
