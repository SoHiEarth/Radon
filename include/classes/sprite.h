#ifndef SPRITE_H
#define SPRITE_H

#include <classes/object.h>
#include <classes/texture.h>
#include <classes/shader.h>
#include <string>

class Sprite : public Object {
  public:
    Sprite() {};
    void Init() override;
    void Update() override;
    void Render() override;
    void Quit() override;
    std::string path;
    Texture* texture = nullptr;
    Shader* shader = nullptr;
  private:
    std::string prev_path;
};

#endif // SPRITE_H
