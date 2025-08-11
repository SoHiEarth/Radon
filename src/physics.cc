#include <engine/physics.h>
#include <box2d/box2d.h>
#include <fmt/core.h>
#define GRAVITY -10.0f // TEMP!!!
#define UPDATE_RATE 60.0f
b2WorldId world_id;

float time_step = 1.0f/UPDATE_RATE;
int substep_count = 4;

void p::Init() {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = (b2Vec2){0.0f, GRAVITY};
  world_id = b2CreateWorld(&world_def);
  fmt::print("Initialized Physics\n");
}

void p::Update() {
  b2World_Step(world_id, time_step, substep_count);
}

void p::Quit() {
  b2DestroyWorld(world_id);
  fmt::print("Terminated Physics\n");
}
