#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include <string>

class Texture;
class Shader;

struct Material {
  std::unique_ptr<Texture> diffuse_, specular_;
  std::unique_ptr<Shader> shader_;
  float shininess_ = 0;
  [[nodiscard]] bool IsValid() const;
  void Bind() const;
};

#endif  // MATERIAL_H
