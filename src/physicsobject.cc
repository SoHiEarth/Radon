#include <classes/object.h>
#include <classes/physicsobject.h>
#include <classes/transform.h>
#include <engine/physics.h>

void PhysicsObject::Init() {
  const auto& position = parent_->transform_.position_;
  const auto& scale = parent_->transform_.scale_;
  physics_body_ = IPhysics::CreateBody(glm::vec2(position->x, position->y),
                                                       static_cast<glm::vec2>(scale));
}

void PhysicsObject::Update() {
  const glm::vec2 kPhysPos = IPhysics::GetBodyPosition(physics_body_);
  parent_->transform_.position_ = glm::vec3(kPhysPos, parent_->transform_.position_->z);
}