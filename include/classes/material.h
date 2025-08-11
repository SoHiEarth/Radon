#ifndef MATERIAL_H
#define MATERIAL_H
#include <string>
class Texture;
class Shader;
struct Material {
  Texture* diffuse = nullptr,
    *specular = nullptr;
  Shader* shader = nullptr;
  float shininess;
  std::string directory;
  const bool IsValid() const;
  void Bind() const;
  void Load(const std::string& path);
  void Free();
  Material(const std::string& path) {
    Load(path);
  }
};

#endif // MATERIAL_H
