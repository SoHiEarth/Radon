#ifndef OBJECT_H
#define OBJECT_H

#include <box2d/box2d.h>
#include <classes/editable.h>
#include <memory>
#include <pugixml.hpp>
#include <string>
#include <vector>
#include <classes/transform.h>

class Component;
class Object : public std::enable_shared_from_this<Object> {
public:
  Transform transform_;
  std::vector<IEditable*> reg_;
  Editable<std::string> name_ = {"Object", "Name", reg_};
  bool has_initialized_ = false;
  bool has_quit_ = false;
  void Init();
  void Update();
  void Render();
  void Quit();
  void Load(pugi::xml_node& /*node*/);
  void Save(pugi::xml_node& /*node*/) const;
  void AddComponent(std::shared_ptr<Component> component);
  template <typename T>
  std::weak_ptr<T> GetComponent() {
    for (auto& component : components_) {
      if (auto casted = std::dynamic_pointer_cast<T>(component)) {
        return casted;
      }
    }
    return {};
  }
  void RemoveComponent(std::shared_ptr<Component> component);
  const std::vector<std::shared_ptr<Component>>& GetAllComponents() {
    return components_;
  }
  virtual ~Object() = default;
private:
  std::vector<std::shared_ptr<Component>> components_ = {};
};

#endif  // OBJECT_H
