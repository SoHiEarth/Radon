#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <classes/component.h>
#include <memory>
#include <string>

class Mesh;
class Material;
class MeshRenderer : public Component {
public:
  ATTR_HAS_MATERIAL;
  ATTR_HAS_MESH;
  Material* material_ = nullptr;
  Model* mesh_ = nullptr;
  void Update() override;
  void Render() override;
  void Load(pugi::xml_node& /*unused*/) override;
  void Save(pugi::xml_node& /*unused*/) const override;
  [[nodiscard]] std::string GetTypeName() const override {
    return "MeshRenderer";
  }
};

#endif  // MESHRENDERER_H