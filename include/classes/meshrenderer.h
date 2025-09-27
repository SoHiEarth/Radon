#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <classes/component.h>
#include <memory>
#include <string>

class Material;
class MeshRenderer : public Component {
public:
  ATTR_HAS_MATERIAL;
  void Update() override;
  void Render() override;
  void Load(pugi::xml_node& /*unused*/) override;
  void Save(pugi::xml_node& /*unused*/) const override;
  std::shared_ptr<Material> material_;
  [[nodiscard]] std::string GetTypeName() const override {
    return "MeshRenderer";
  }
};

#endif  // MESHRENDERER_H