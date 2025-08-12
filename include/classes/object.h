#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include <string>
#include <classes/editable.h>

class Object {
  public:
    std::vector<IEditable*> reg;
    Editable<std::string> name    = {"Object", "Name", reg};
    Editable<glm::vec3> position  = {glm::vec3(0.0f), "Position", reg};
    Editable<glm::vec2> scale     = {glm::vec2(1.0f), "Scale", reg};
    Editable<float> rotation      = {0.0f, "Rotation", reg};
    Editable<bool> is_static      = {false, "Static", reg};
    virtual void Init()   = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void Quit()   = 0;
};

#endif // OBJECT_H
