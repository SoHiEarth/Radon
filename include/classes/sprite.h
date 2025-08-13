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
  Editable<std::string> path_ = {"sprite_test", "Path", reg_};

private:
  std::string prev_path_;
};

#endif  // SPRITE_H
