#include <box2d/box2d.h>
#include <engine/physics.h>
#include <fmt/core.h>
#define GRAVITY -10.0f
#define UPDATE_RATE 60.0f

b2WorldId g_world_id;
float g_time_step = 1.0F / UPDATE_RATE;
int g_substep_count = 4;

void p::Init() {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = (b2Vec2) {0.0F, GRAVITY};
  g_world_id = b2CreateWorld(&world_def);
  fmt::print("Initialized Physics\n");
}

void p::Update() {
  b2World_Step(g_world_id, g_time_step, g_substep_count);
}

void p::Quit() {
  b2DestroyWorld(g_world_id);
  fmt::print("Terminated Physics\n");
}
