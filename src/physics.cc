import metal.debug;
import metal.physics;
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <vector>

namespace physics {
const float TIME_STEP = 1.0f / 60.0f;
const int VELOCITY_ITERATIONS = 6;
const int POSITION_ITERATIONS = 2;
b2WorldId world;

void Init() {
  b2WorldDef worldDef = b2DefaultWorldDef();
  worldDef.gravity = b2Vec2(0.0f, -1.0f);
  world = b2CreateWorld(&worldDef);
  debug::Log("Initialized physics");
}

void Update() {
  b2World_Step(world, TIME_STEP, VELOCITY_ITERATIONS);
}

b2BodyId CreateBody(glm::vec2 position, glm::vec2 scale) {
  b2BodyDef bodyDef = b2DefaultBodyDef();
  bodyDef.type = b2_dynamicBody;
  bodyDef.position = b2Vec2(position.x, position.y);
  b2BodyId body = b2CreateBody(world, &bodyDef);
  b2Polygon shape = b2MakeBox(scale.x * 0.5f, scale.y * 0.5f);
  b2ShapeDef shapeDef = b2DefaultShapeDef();
  shapeDef.density = 1.0f;
  shapeDef.material.friction = 0.3f;
  b2CreatePolygonShape(body, &shapeDef, &shape);
  return body;
}

glm::vec2 GetBodyPosition(b2BodyId body_id) {
  b2Vec2 position = b2Body_GetPosition(body_id);
  return glm::vec2(position.x, position.y);
}

void SetBodyPosition(b2BodyId body_id, glm::vec2 position) {
  b2Body_SetTransform(body_id, b2Vec2(position.x, position.y), b2Body_GetRotation(body_id));
}

void Quit() {
  b2DestroyWorld(world);
  debug::Log("Quit physics");
}

}  // namespace physics
