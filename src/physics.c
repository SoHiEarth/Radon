#include <box2d/box2d.h>
#include <engine/physics.h>
#include <stdio.h>
#define GRAVITY -10.0f
#define UPDATE_RATE 60.0f

b2WorldId g_world_id;
float g_time_step = 1.0F / UPDATE_RATE;
int g_substep_count = 4;

void PInit() {
  b2WorldDef world_def = b2DefaultWorldDef();
  world_def.gravity = (b2Vec2) {0.0F, GRAVITY};
  g_world_id = b2CreateWorld(&world_def);
  printf("Initialized Physics\n");
}

void PUpdate() {
  b2World_Step(g_world_id, g_time_step, g_substep_count);
}

void PQuit() {
  b2DestroyWorld(g_world_id);
  printf("Terminated Physics\n");
}
