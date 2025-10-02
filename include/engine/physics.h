#pragma once
#include <box2d/box2d.h>
#include <engine/interface.h>
#include <glm/glm.hpp>
class IPhysics : public Interface {
protected:
  void IInit() override;
  void IUpdate() override;
  void IQuit() override;

public:
  const char* Name() override {
    return "Physics";
  }
  static b2BodyId CreateBody(glm::vec2 /*position*/, glm::vec2 /*scale*/);
  static glm::vec2 GetBodyPosition(b2BodyId /*body_id*/);
  static void SetBodyPosition(b2BodyId /*body_id*/, glm::vec2 /*position*/);
};