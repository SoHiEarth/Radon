#ifndef MATERIAL_H
#define MATERIAL_H
#include <classes/texture.h>
struct Material {
  Texture* diffuse;
  Texture* specular;
  float shininess;
  Material() {
    diffuse = new Texture();
    specular = new Texture();
  }
};

#endif // MATERIAL_H
