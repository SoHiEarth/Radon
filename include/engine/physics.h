#ifndef PHYSICS_H
#define PHYSICS_H
#include <box2d/box2d.h>
#include <glm/glm.hpp>
namespace physics {
void Init();
void Update();
b2BodyId CreateBody(glm::vec2 position, glm::vec2 scale);
glm::vec2 GetBodyPosition(b2BodyId body_id);
void SetBodyPosition(b2BodyId body_id, glm::vec2 position);
void Quit();
};  // namespace physics
#endif  // PHYSICS_H
