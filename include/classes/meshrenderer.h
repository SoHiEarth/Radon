#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include <classes/component.h>
#include <memory>
#include <string>

class Material;
class MeshRenderer : public Component {
public:
  ATTR_HAS_MATERIAL;
  void Init() override;
  void Update() override;
  void Render() override;
  void Quit() override;
  void Load(pugi::xml_node& /*node*/ /*unused*/) override;
  void Save(pugi::xml_node& /*node*/ /*unused*/) const override;
  ~MeshRenderer() = default;
  std::shared_ptr<Material> material_;
  [[nodiscard]] std::string GetTypeName() const override {
    return "MeshRenderer";
  }
};

#endif  // MESHRENDERER_H