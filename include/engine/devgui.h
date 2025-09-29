#pragma once
#include <engine/interface.h>
#include <string>
#include <cstdint>
enum ConsoleMessageType : std::uint8_t {
  kConsoleMessageTypeInfo = 0,
  kConsoleMessageTypeWarning = 1,
  kConsoleMessageTypeError = 2
};

struct ConsoleMessage {
  std::string traceback_;
  std::string message_;
  ConsoleMessageType type_;
};

class IGui : public Interface {
  bool hud_enabled = false;
public:
  const char* name() override {
    return "Gui";
  }
  void SetHud(bool enabled) {
    hud_enabled = enabled;
  }
  bool GetHud() const {
    return hud_enabled;
  }
  void Init() override;
  void Update();
  static void AddConsoleMessage(const char* traceback, const char* message, std::uint8_t type);
  void Render();
  void Quit() override;
};