#ifndef INPUT_H
#define INPUT_H

#include <functional>
#define Trigger std::pair<int, ButtonState>

enum class ButtonState {
  PRESS, // Button was pressed this frame
  HOLD, // Button is being held down
  RELEASE // Button was released this frame
};

namespace i {
  void AddHook(const Trigger& trigger, const std::function<void()>& hook);
  void RemoveHook(const Trigger& trigger);
  void Init();
  void Update();
  void Quit();
}

#endif // INPUT_H
