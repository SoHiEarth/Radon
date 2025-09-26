#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include <classes/component.h>
#include <memory>
#include <string>
#include <engine/physics.h>

class PhysicsObject : public Component {
public:
  // Editable<float> friction_ = {0.0F, "Friction", reg_};
  void Init() override;
  void Update() override;
  void Render() override;
  void Quit() override;
  void Load(pugi::xml_node& /*node*/ /*unused*/) override;
  void Save(pugi::xml_node& /*node*/ /*unused*/) const override;
  ~PhysicsObject() = default;
  b2BodyId physics_body_;
  [[nodiscard]] std::string GetTypeName() const override {
    return "PhysicsObject";
  }
};

#endif  // PHYSICSOBJECT_H