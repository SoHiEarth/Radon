#pragma once

class Interface {
private:
  bool started = false;
protected:
  virtual void i_Init() {};
  virtual void i_Update() {};
  virtual void i_Render() {};
  virtual void i_Quit() {};
public:
  template <typename T>
  static T& Get() {
    static_assert(std::is_base_of<Interface, T>::value, "T must derive from Interface");
    static T instance;
    return instance;
  }

  static std::vector<Interface*>& All();

  virtual const char* name() {
    return "Interface";
  }

  bool GetStatus() const {
    return started;
  }
  void Start();
  void Update();
  void Render();
  void Stop();
};