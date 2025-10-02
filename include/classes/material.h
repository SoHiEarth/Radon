#pragma once
class Texture;
class Shader;
struct Material {
  Texture* diffuse_ = nullptr;
  Texture* specular_ = nullptr;
  Shader* shader_ = nullptr;
  float shininess_ = 0;
};
