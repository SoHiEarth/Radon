#include <classes/object.h>
#include <classes/physicsobject.h>
#include <classes/transform.h>
#include <engine/physics.h>

void PhysicsObject::Init() {
  const auto& position_ = parent_->transform_.position_;
  const auto& scale_ = parent_->transform_.scale_;
  physics_body_ = physics::CreateBody(glm::vec2(position_->x, position_->y), static_cast<glm::vec2>(scale_));
}

void PhysicsObject::Update() {
  const glm::vec2 kPhysPos = physics::GetBodyPosition(physics_body_);
  parent_->transform_.position_ = glm::vec3(kPhysPos, parent_->transform_.position_->z);
}