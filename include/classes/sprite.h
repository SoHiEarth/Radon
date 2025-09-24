#ifndef SPRITE_H
#define SPRITE_H

#include <classes/component.h>
#include <string>
#include <memory>

class Material;
class Sprite : public Component {
public:
  ATTR_HAS_MATERIAL;
  void Init() override;
  void Update() override;
  void Render() override;
  void Quit() override;
  void Load(pugi::xml_node&) override;
  void Save(pugi::xml_node&) const override;
  ~Sprite() = default;
  std::shared_ptr<Material> material_;
  [[nodiscard]] std::string GetTypeName() const override {
    return "Sprite";
  }
};

#endif  // SPRITE_H
