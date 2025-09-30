#pragma once
#include <engine/interface.h>
#include <cstdint>
#include <functional>

enum class ButtonState : std::uint8_t {
  kPress = 0,   // Button was pressed this frame
  kHold = 1,    // Button is being held down
  kRelease = 2  // Button was released this frame
};

using Trigger = std::pair<int, ButtonState>;

class IInput : public Interface {
protected:
  void i_Init() override;
  void i_Update() override;
  void i_Quit() override;

public:
  const char* name() override {
    return "Input";
  }
  void AddHook(const Trigger& /*key*/, const std::function<void()>& /*hook*/);
  void RemoveHook(const Trigger& /*key*/);
};