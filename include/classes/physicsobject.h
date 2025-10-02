#pragma once
#include <classes/component.h>
#include <engine/physics.h>

class PhysicsObject : public Component {
public:
  b2BodyId physics_body_ = {};
  void Init() override;
  void Update() override;
  [[nodiscard]] const char* GetTypeName() const override {
    return "PhysicsObject";
  }
};
