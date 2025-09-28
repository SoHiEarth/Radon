#ifndef OBJECT_H
#define OBJECT_H

#include <classes/editable.h>
#include <classes/transform.h>
#include <memory>
#include <pugixml.hpp>
#include <string>
#include <vector>

class Component;
class Object {
public:
  Transform transform_{};
  std::vector<IEditable*> reg_;
  Editable<std::string> name_ = {"Object", "Name", reg_};
  bool has_initialized_ = false;
  bool has_quit_ = false;
  void Init();
  void Update();
  void Render();
  void Quit();
  void Load(pugi::xml_node&);
  void Save(pugi::xml_node&) const;
  void AddComponent(Component*);
  template <typename T>
  T*& GetComponent() {
    for (auto& component : components_) {
      if (auto casted = std::dynamic_pointer_cast<T>(component)) {
        return casted;
      }
    }
    return {};
  }
  void RemoveComponent(Component*);
  const std::vector<Component*>& GetAllComponents() const {
    return components_;
  }
  virtual ~Object() = default;

private:
  std::vector<Component*> components_;
};

#endif  // OBJECT_H
