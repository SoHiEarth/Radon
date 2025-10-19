#pragma once
#include <engine/interface.h>
#include <cstdint>
#include <string>
enum ConsoleMessageType : std::uint8_t {
  kConsoleMessageTypeInfo = 0,
  kConsoleMessageTypeWarning = 1,
  kConsoleMessageTypeError = 2
};

struct ConsoleMessage {
  const std::string traceback_;
  const std::string message_;
  ConsoleMessageType type_;
};

class IGui : public Interface {
private:
  bool hud_enabled_ = false;

protected:
  void IInit() override;
  void IUpdate() override;
  void IRender() override;
  void IQuit() override;

public:
  const char* Name() override {
    return "Gui";
  }
  void SetHud(bool enabled) {
    hud_enabled_ = enabled;
  }
  [[nodiscard]] bool GetHud() const {
    return hud_enabled_;
  }
  static void AddConsoleMessage(std::string_view traceback, std::string_view message,
                                std::uint8_t type);
};