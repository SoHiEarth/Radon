#pragma once
class Interface {
  virtual void Init() {};
  virtual void Quit() {};
public:
  template <typename T>
  static T& Get() {
    static_assert(std::is_base_of<Interface, T>::value, "T must derive from Interface");
    static T instance;
    return instance;
  }

  virtual const char* name() {
    return "Interface";
  }
  void Start();
  void Stop();
};