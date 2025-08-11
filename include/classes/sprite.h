#ifndef SPRITE_H
#define SPRITE_H

#include <classes/object.h>
#include <string>

class Material;

class Sprite : public Object {
  public:
    void Init() override;
    void Update() override;
    void Render() override;
    void Quit() override;
    std::string path;
    Material* material = nullptr;
  private:
    std::string prev_path;
};

#endif // SPRITE_H
