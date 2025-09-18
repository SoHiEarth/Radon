export module metal.material;
import metal.texture;
import metal.shader;

export struct Material {
  bool is_initialized_ = false;
  Texture *diffuse_ = nullptr, *specular_ = nullptr;
  Shader* shader_ = nullptr;
  float shininess_ = 0;
  [[nodiscard]] bool IsValid() const;
  void Bind() const;
};