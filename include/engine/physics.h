#pragma once
#include <engine/interface.h>
#include <box2d/box2d.h>
#include <glm/glm.hpp>
class IPhysics : public Interface {
protected:
  const char* name() override {
    return "Physics";
  }
  void i_Init() override;
  void i_Update() override;
  void i_Quit() override;

public:
  b2BodyId CreateBody(glm::vec2 /*position*/, glm::vec2 /*scale*/);
  glm::vec2 GetBodyPosition(b2BodyId /*body_id*/);
  void SetBodyPosition(b2BodyId /*body_id*/, glm::vec2 /*position*/);
};