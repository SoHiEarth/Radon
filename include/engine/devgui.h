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
  const char* traceback_;
  const char* message_;
  ConsoleMessageType type_;
};

class IGui : public Interface {
private:
  bool hud_enabled = false;
protected:
  const char* name() override {
    return "Gui";
  }
  void i_Init() override;
  void i_Update() override;
  void i_Render() override;
  void i_Quit() override;
public:
  void SetHud(bool enabled) {
    hud_enabled = enabled;
  }
  bool GetHud() const {
    return hud_enabled;
  }
  static void AddConsoleMessage(const char* traceback, const char* message, std::uint8_t type);
};