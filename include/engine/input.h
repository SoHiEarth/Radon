#ifndef INPUT_H
#define INPUT_H

#include <cstdint>
#include <functional>

enum class ButtonState : std::uint8_t {
  kPress,   // Button was pressed this frame
  kHold,    // Button is being held down
  kRelease  // Button was released this frame
};

using Trigger = std::pair<int, ButtonState>;

namespace input {
void Init();
void Update();
void Quit();
void AddHook(const Trigger&, const std::function<void()>&);
void RemoveHook(const Trigger&);
}  // namespace input

#endif  // INPUT_H
