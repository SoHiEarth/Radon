module;
#include <box2d/box2d.h>
#include <glm/glm.hpp>
export module metal.physics;

export namespace physics {
void Init();
void Update();
b2BodyId CreateBody(glm::vec2 position, glm::vec2 scale);
glm::vec2 GetBodyPosition(b2BodyId body_id);
void SetBodyPosition(b2BodyId body_id, glm::vec2 position);
void Quit();
};  // namespace physics