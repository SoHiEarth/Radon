#ifndef OBJECT_H
#define OBJECT_H

#include <box2d/box2d.h>
#include <classes/editable.h>
#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <string>
#include <vector>
#include <memory>

class Component;
class Material;
class Object : public std::enable_shared_from_this<Object> {
public:
  std::vector<IEditable*> reg_;
  Editable<bool> is_static_ = {false, "Static", reg_};
  Editable<std::string> name_ = {"Object", "Name", reg_};
  std::vector<std::unique_ptr<Component>> components_;
  // Will be replaced with a Transform component later
  Editable<glm::vec3> position_ = {glm::vec3(0.0F), "Position", reg_};
  Editable<glm::vec2> scale_ = {glm::vec2(1.0F), "Scale", reg_};
  Editable<glm::vec3> rotation_ = {glm::vec3(0.0F), "Rotation", reg_};
  b2BodyId physics_body_;  // Will be replaced with a physics component later
  bool has_initialized_ = false;
  void Init();
  void Update();
  void Render();
  bool has_quit_ = false;
  void Quit();
  void Load(pugi::xml_node&);
  void Save(pugi::xml_node&) const;
  void AddComponent(std::unique_ptr<Component> component);
  template <typename T>
  std::weak_ptr<T> GetComponent() {
    for (auto& component : components_) {
      if (auto casted = std::dynamic_pointer_cast<T>(component)) {
        return casted;
      }
    }
    return nullptr;
  }

  void RemoveComponent(std::weak_ptr<Component> component);
  virtual ~Object() = default;
};

#endif  // OBJECT_H
