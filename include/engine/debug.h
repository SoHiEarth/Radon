#ifndef DEBUG_H
#define DEBUG_H
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
  using location = std::source_location;
public:
  const char* name() override {
    return "Debug";
  }
  DebugSettings& Settings() {
    return g_debug_settings;
  }
  void SetCallback(std::function<void(const char*, const char*, std::uint8_t)> /*func*/) noexcept;
  static void Log(const char* /*msg*/, const location& /*loc*/ = location::current()) noexcept;
  static void Log(std::string_view /*msg*/, const location& /*loc*/ = location::current()) noexcept;
  static void Warning(const char* /*msg*/, const location& /*loc*/ = location::current()) noexcept;
  static void Warning(std::string_view /*msg*/, const location& /*loc*/ = location::current()) noexcept;
  static void Throw(const char* /*msg*/, const location& /*loc*/ = location::current());
  static void Throw(std::string_view /*msg*/, const location& /*loc*/ = location::current());
};

#endif  // DEBUG_H
