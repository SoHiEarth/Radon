#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include <classes/component.h>
#include <engine/audio.h>

class AudioSource : public Component {
public:
  Editable<std::string> path = {"", "Path", reg_};
  void Init() override;
  void Update() override;
  void Quit() override;
  void Load(pugi::xml_node& node) override;
  void Save(pugi::xml_node& node) const override;
  [[nodiscard]] std::string GetTypeName() const override {
    return "AudioSource";
  }
private:
  SoundHandle handle{};
};

#endif  // AUDIOSOURCE_H