#ifndef COMPONENT_H
#define COMPONENT_H

struct ComponentData {
  void Load();
  void Save();
};

class Component {
  public:
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Quit() = 0;
    virtual ~Component() = default; // Ensure proper cleanup of derived classes
  private:
    ComponentData data;
};

#endif // COMPONENT_H
