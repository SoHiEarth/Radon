module;
#include <string>
#include <pugixml.hpp>
export module metal.sprite;
import metal.object;

export class Sprite : public Object {
public:
  void Init() override;
  void Update() override;
  void Render() override;
  void Quit() override;
  void Load(pugi::xml_node& /*node*/ /*unused*/) override;
  void Save(pugi::xml_node& /*node*/ /*unused*/) const override;
  ~Sprite() = default;
  [[nodiscard]] std::string GetTypeName() const override {
    return "Sprite";
  }
};