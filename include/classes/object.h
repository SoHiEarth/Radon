#ifndef OBJECT_H
#define OBJECT_H

#include <vector>

class Component;

class Object {
  public:
    bool is_static = false;
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Quit() = 0;
    std::vector<Component> components;
};

#endif // OBJECT_H
