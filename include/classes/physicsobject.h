#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include <classes/component.h>
#include <memory>
#include <string>
#include <engine/physics.h>

class PhysicsObject : public Component {
public:
  void Init() override;
  void Update() override;
  b2BodyId physics_body_ = {};
  [[nodiscard]] std::string GetTypeName() const override {
    return "PhysicsObject";
  }
};

#endif  // PHYSICSOBJECT_H