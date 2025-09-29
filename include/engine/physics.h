#pragma once
#include <engine/interface.h>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
class IPhysics : public Interface {
public:
  const char* name() override {
    return "Physics";
  }
  void Init() override;
  void Update();
  void Quit() override;
  b2BodyId CreateBody(glm::vec2 /*position*/, glm::vec2 /*scale*/);
  glm::vec2 GetBodyPosition(b2BodyId /*body_id*/);
  void SetBodyPosition(b2BodyId /*body_id*/, glm::vec2 /*position*/);
};