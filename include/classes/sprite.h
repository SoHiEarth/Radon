#ifndef SPRITE_H
#define SPRITE_H

#include <classes/object.h>
#include <string>

class Sprite : public Object {
public:
  void Init() override;
  void Update() override;
  void Render() override;
  void Quit() override;
  void Load(pugi::xml_node& /*node*/ /*unused*/) override;
  [[nodiscard]] std::string GetTypeName() const override {
    return "Sprite";
  }
  void Save(pugi::xml_node& /*node*/ /*unused*/) const override;
  ~Sprite() = default;

private:
};

#endif  // SPRITE_H
