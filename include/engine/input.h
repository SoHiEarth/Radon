#pragma once
#include <engine/interface.h>
#include <cstdint>
#include <functional>

enum class ButtonState : std::uint8_t {
  kPress,   // Button was pressed this frame
  kHold,    // Button is being held down
  kRelease  // Button was released this frame
};

using Trigger = std::pair<int, ButtonState>;

class IInput : public Interface {
public:
  const char* name() override {
    return "Input";
  }
  void Init() override;
  void Update();
  void Quit() override;
  void AddHook(const Trigger& /*key*/, const std::function<void()>& /*hook*/);
  void RemoveHook(const Trigger& /*key*/);
};