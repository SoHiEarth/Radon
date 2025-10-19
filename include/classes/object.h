#pragma once
#include <classes/editable.h>
#include <classes/transform.h>
#include <memory>
#include <pugixml.hpp>
#include <vector>
#include <memory>

class Component;
class Object {
private:
  std::vector<std::unique_ptr<Component>> components_;
  Engine* engine_;

public:
  Transform transform_{};
  std::vector<IEditable*> reg_{};
  Editable<std::string> name_ = {"Object", "Name", reg_};
  bool has_initialized_ = false;
  bool has_quit_ = false;
  void Init();
  void Update();
  void Render();
  void Quit();
  void Load(pugi::xml_node& /*node*/);
  void Save(pugi::xml_node& /*node*/) const;
  void AddComponent(std::unique_ptr<Component>& /*component*/);
  template <typename T>
  T*& GetComponent() {
    for (auto& component : components_) {
      if (auto casted = std::dynamic_pointer_cast<T>(component)) {
        return casted;
      }
    }
    return {};
  }
  void RemoveComponent(const std::unique_ptr<Component>& /*component*/);
  [[nodiscard]] const std::vector<std::unique_ptr<Component>>& GetAllComponents() const {
    return components_;
  }
  explicit Object(Engine* engine) : engine_(engine) {
    transform_.SetEngineInstance(engine);  // Set engine for transform
  }
  virtual ~Object() = default;
};
