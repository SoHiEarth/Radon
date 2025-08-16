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
  void Load(pugi::xml_node&) override;
  [[nodiscard]] std::string GetTypeName() const override {
    return "Sprite";
  }
  void Save(pugi::xml_node&) const override;
  Editable<std::string> path_ = {"sprite_test", "Path", reg_};
  ~Sprite() = default;

private:
  std::string prev_path_;
};

#endif  // SPRITE_H
