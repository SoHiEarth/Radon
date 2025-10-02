#pragma once
#include <fmt/core.h>
#include <vector>

enum class InterfaceState : std::uint8_t { kInitialized = 1, kStopped = 2, kError = 3 };

class Interface {
protected:
  InterfaceState state_ = InterfaceState::kStopped;
  virtual void IInit() {};
  virtual void IUpdate() {};
  virtual void IRender() {};
  virtual void IQuit() {};

public:
  virtual const char* Name() {
    return "Interface";
  }

  static std::vector<Interface*>& All();
  template <typename T>
  static T& Get() {
    static_assert(std::is_base_of_v<Interface, T>, "T must derive from Interface");
    static T instance;
    return instance;
  }

  [[nodiscard]] InterfaceState GetState() const {
    return state_;
  }

  void Start();
  void Update();
  void Render();
  void Stop();
};
