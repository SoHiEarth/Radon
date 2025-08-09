#ifndef SPRITE_H
#define SPRITE_H

#include <classes/component.h>
#include <string>
#include <classes/texture.h>

class SpriteData : public ComponentData {
  public:
    void Load();
    void Save();
    Texture texture;
    std::string texture_path;
};

class Sprite : public Component {
  public:
    Sprite();
    void Init() override;
    void Update() override;
    void Quit() override;
    void SetTexture(const std::string& path);
    Texture GetTexture() const;

  private:
    SpriteData data;
};

#endif // SPRITE_H
