#ifndef OBJECT_H
#define OBJECT_H

#include <box2d/box2d.h>
#include <classes/editable.h>
#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <string>
#include <vector>

class Component;
class Material;
class Object {
public:
  std::vector<IEditable*> reg_;
  Editable<bool> is_static_ = {false, "Static", reg_};
  Editable<std::string> name_ = {"Object", "Name", reg_};
  std::vector<Component*> components_;
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
  void AddComponent(Component* component);
  template <typename T>
  T* GetComponent() {
    for (auto* component : components_) {
      if (std::is_same_v<T, Component>) {
        return static_cast<T*>(component);
      }
    }
    return nullptr;
  }

  void RemoveComponent(Component* component);
  virtual ~Object() = default;
};

#endif  // OBJECT_H
