#ifndef SPRITE_H
#define SPRITE_H

#include <classes/object.h>
#include <classes/texture.h>
#include <string>

class Sprite : public Object {
  public:
    Sprite();
    void Init() override;
    void Update() override;
    void Render() override;
    void Quit() override;
    void SetTexture(const std::string& path);
    Texture GetTexture() const;
  private:
    Texture texture;
    std::string path;
};

#endif // SPRITE_H
