#ifndef OBJECT_H
#define OBJECT_H
#include <glm/glm.hpp>
class Object {
  public:
    glm::vec3 position{0, 0, 0};
    glm::vec2 scale{1,1};
    float rotation = 0.0f;
    bool is_static = false;
    virtual void Init() {};
    virtual void Update() {};
    virtual void Render() {};
    virtual void Quit() {};
};
#endif // OBJECT_H
