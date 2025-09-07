#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

class Texture;
class Shader;

struct Material {
  Texture *diffuse_ = nullptr, *specular_ = nullptr;
  Shader* shader_ = nullptr;
  float shininess_ = 0;
  [[nodiscard]] bool IsValid() const;
  void Bind() const;
};

#endif  // MATERIAL_H
