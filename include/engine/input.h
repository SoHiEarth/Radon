#ifndef INPUT_H
#define INPUT_H

#include <cstdint>
#include <functional>
#define Trigger std::pair<int, ButtonState>

enum class ButtonState : std::uint8_t {
  kPress,   // Button was pressed this frame
  kHold,    // Button is being held down
  kRelease  // Button was released this frame
};

namespace i {
void AddHook(const Trigger& key, const std::function<void()>& hook);
void RemoveHook(const Trigger& key);
void Init();
void Update();
void Quit();
}  // namespace i

#endif  // INPUT_H
