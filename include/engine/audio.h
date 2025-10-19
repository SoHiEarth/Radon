#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <engine/interface.h>
#include <map>
#include <string>

class IAudio : public Interface {
private:
  ALCdevice* g_device_ = nullptr;
  ALCcontext* g_context_ = nullptr;
  struct SoundData {
    ALuint buffer_{};
    ALuint source_{};
    ALsizei frequency_{};
    bool is_playing_ = false;
  };
  std::map<unsigned int, SoundData> g_sounds_;
  unsigned int g_next_ = 1;

protected:
  void IInit() override;
  void IUpdate() override;
  void IQuit() override;
  void LoadWAV(const char* filename, std::vector<char>& data, ALenum& format,
    ALsizei& frequency);

public:
  const char* Name() override {
    return "Audio";
  }
  unsigned int Load(std::string_view /*filepath*/);
  void Unload(unsigned int /*sound*/);
  void PlaySound(unsigned int /*sound*/);
  void StopSound(unsigned int /*sound*/);
  void SetHeader(unsigned int /*sound*/, float /*position*/);
};
