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
    Editable<std::string> path = {"sprite_test", "Path", reg};
  private:
    std::string prev_path;
};

#endif // SPRITE_H
