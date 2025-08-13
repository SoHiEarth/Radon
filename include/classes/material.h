#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

class Texture;
class Shader;

struct Material {
  Texture *diffuse_ = nullptr, *specular_ = nullptr;
  Shader* shader_ = nullptr;
  float shininess_ = 0;
  std::string directory_;
  [[nodiscard]] bool IsValid() const;
  void Bind() const;
  void Load(const std::string& path);
  void Free() const;
  explicit Material(const std::string& path) {
    Load(path);
  }
};

#endif  // MATERIAL_H
