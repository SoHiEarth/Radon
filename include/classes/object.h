#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <string>

class Object {
  public:
    std::string name = "Object";
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec2 scale = glm::vec2(1.0f);
    float rotation = 0.0f;
    bool is_static = false;
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void Quit() = 0;
};

#endif // OBJECT_H
