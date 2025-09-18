module;
#include <cstdint>
#include <functional>
export module metal.input;

export enum class ButtonState : std::uint8_t {
  kPress,   // Button was pressed this frame
  kHold,    // Button is being held down
  kRelease  // Button was released this frame
};

export namespace input {
void Init();
void Update();
void Quit();
void AddHook(const std::pair<int, ButtonState>& /*key*/, const std::function<void()>& /*hook*/);
void RemoveHook(const std::pair<int, ButtonState>& /*key*/);
}  // namespace input