#ifndef MODELRENDERER_H
#define MODELRENDERER_H

#include <classes/component.h>
#include <string>

class Mesh;
class Material;
class ModelRenderer : public Component {
public:
  ATTR_HAS_MATERIAL;
  ATTR_HAS_MODEL;
  Model* model_ = nullptr;
  Material* material_ = nullptr;
  void Render() override;
  void Load(pugi::xml_node& /*unused*/) override;
  void Save(pugi::xml_node& /*unused*/) const override;
  [[nodiscard]] std::string GetTypeName() const override {
    return "ModelRenderer";
  }
};

#endif  // MODELRENDERER_H