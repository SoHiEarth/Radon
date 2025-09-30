#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <engine/interface.h>
#include <map>

class IAudio : public Interface {
private:
  ALCdevice* g_device = nullptr;
  ALCcontext* g_context = nullptr;
  struct SoundData {
    ALuint buffer_{};
    ALuint source_{};
    ALsizei frequency_{};
    bool is_playing_ = false;
  };
  std::map<unsigned int, SoundData> g_sounds;
  unsigned int g_next = 1;

protected:
  void i_Init() override;
  void i_Update() override;
  void i_Quit() override;

public:
  const char* name() override {
    return "Audio";
  }
  unsigned int Load(const char* /*filepath*/);
  void Unload(unsigned int /*sound*/);
  void PlaySound(unsigned int /*sound*/);
  void StopSound(unsigned int /*sound*/);
  void SetHeader(unsigned int /*sound*/, float /*position*/);
};
