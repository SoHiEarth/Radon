#include <box2d/box2d.h>
#include <engine/debug.h>
#include <engine/physics.h>
#include <glm/glm.hpp>
#include <vector>

const float kTimeStep = 1.0F / 60.0F;
const int kVelocityIterations = 6;
const int kPositionIterations = 2;
b2WorldId g_world;

void IPhysics::i_Init() {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = b2Vec2(0.0F, -1.0F);
  g_world = b2CreateWorld(&world_def);
}

void IPhysics::i_Update() {
  b2World_Step(g_world, kTimeStep, kVelocityIterations);
}

b2BodyId IPhysics::CreateBody(glm::vec2 position, glm::vec2 scale) {
  b2BodyDef body_def = b2DefaultBodyDef();
  body_def.type = b2_dynamicBody;
  body_def.position = b2Vec2(position.x, position.y);
  b2BodyId body = b2CreateBody(g_world, &body_def);
  b2Polygon shape = b2MakeBox(scale.x * 0.5F, scale.y * 0.5F);
  b2ShapeDef shape_def = b2DefaultShapeDef();
  shape_def.density = 1.0F;
  shape_def.material.friction = 0.3F;
  b2CreatePolygonShape(body, &shape_def, &shape);
  return body;
}

glm::vec2 IPhysics::GetBodyPosition(b2BodyId body_id) {
  b2Vec2 position = b2Body_GetPosition(body_id);
  return {position.x, position.y};
}

void IPhysics::SetBodyPosition(b2BodyId body_id, glm::vec2 position) {
  b2Body_SetTransform(body_id, b2Vec2(position.x, position.y), b2Body_GetRotation(body_id));
}

void IPhysics::i_Quit() {
  b2DestroyWorld(g_world);
}