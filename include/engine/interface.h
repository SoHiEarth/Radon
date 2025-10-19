#pragma once
#include <fmt/core.h>
#include <vector>

enum class InterfaceState : std::uint8_t { kInitialized = 1, kStopped = 2, kError = 3 };
class Engine;

class Interface {
protected:
  InterfaceState state_ = InterfaceState::kStopped;
  virtual void IInit() {};
  virtual void IUpdate() {};
  virtual void IRender() {};
  virtual void IQuit() {};
  Engine* engine_ = nullptr;

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

  void SetEngineInstance(Engine* engine) {
    engine_ = engine;
  }

  [[nodiscard]] InterfaceState GetState() const {
    return state_;
  }

  void Start();
  void Update();
  void Render();
  void Stop();
};
