#ifndef PHYSICS_H
#define PHYSICS_H
#include <box2d/box2d.h>
#include <glm/glm.hpp>
namespace physics {
void Init();
void Update();
b2BodyId CreateBody(glm::vec2, glm::vec2);
glm::vec2 GetBodyPosition(b2BodyId);
void SetBodyPosition(b2BodyId, glm::vec2);
void Quit();
};  // namespace physics
#endif  // PHYSICS_H
