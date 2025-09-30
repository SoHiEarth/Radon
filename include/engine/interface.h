#pragma once
#include <vector>
#include <fmt/core.h>

enum class InterfaceState : std::uint8_t {
  Initialized = 1,
  Stopped = 2,
  Error = 3
};

class Interface {
protected:
  InterfaceState state = InterfaceState::Stopped;
  virtual void i_Init() {};
  virtual void i_Update() {};
  virtual void i_Render() {};
  virtual void i_Quit() {};

public:
  virtual const char* name() {
    return "Interface";
  }

  static std::vector<Interface*>& All();
  template <typename T>
  static T& Get() {
    static_assert(std::is_base_of<Interface, T>::value, "T must derive from Interface");
    static T instance;
    return instance;
  }

  InterfaceState GetState() const {
    return state;
  }

  void Start();
  void Update();
  void Render();
  void Stop();
};
