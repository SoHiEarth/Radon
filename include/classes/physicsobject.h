#pragma once
#include <classes/component.h>
#include <engine/physics.h>

class PhysicsObject : public Component {
public:
  void Init() override;
  void Update() override;
  b2BodyId physics_body_ = {};
  [[nodiscard]] const char* GetTypeName() const override {
    return "PhysicsObject";
  }
};