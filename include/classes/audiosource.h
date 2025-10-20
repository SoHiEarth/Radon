#pragma once
#include <classes/component.h>
#include <engine/audio.h>

class AudioSource : public Component {
public:
  Editable<std::string> path_ = {"", "Path", reg_};
  void Init() override;
  void Update() override;
  void Quit() override;
  void Load(pugi::xml_node& node) override;
  void Save(pugi::xml_node& node) const override;
  [[nodiscard]] const char* GetTypeName() const override {
    return "AudioSource";
  }

private:
  unsigned int handle_;
};

REGISTER_COMPONENT(AudioSource);