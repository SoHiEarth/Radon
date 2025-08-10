#ifndef ENGINE_INPUT_H
#define ENGINE_INPUT_H

#include <functional>

enum class ButtonState {
  BUTTON_STATE_PRESS, // Button was pressed this frame
  BUTTON_STATE_HOLD, // Button is being held down
  BUTTON_STATE_RELEASE // Button was released this frame
                       // Triggered when button is released
};

namespace i {
  void AddHook(std::pair<int, ButtonState>, std::function<void()> hook);
  void RemoveHook(std::pair<int, ButtonState> key);
  void Init();
  void Update();
  void Quit();
}

#endif // ENGINE_INPUT_H
