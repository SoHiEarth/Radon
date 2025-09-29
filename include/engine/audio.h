#ifndef AUDIO_H
#define AUDIO_H
#include <engine/interface.h>
using SoundHandle = unsigned int;

class IAudio : public Interface {
public:
  const char* name() override {
    return "Audio";
  }

  void Init() override;
  void Update();
  void Quit() override;

  SoundHandle Load(const char* /*filepath*/);
  void Unload(SoundHandle /*sound*/);
  // Schedules the sound to be played on the next update, until stopped with audio::Stop
  void PlaySound(SoundHandle /*sound*/);
  // Stops the sound if it is scheduled to play.
  void StopSound(SoundHandle /*sound*/);
  // Sets the time position of the sound in seconds.
  void SetHeader(SoundHandle /*sound*/, float /*position*/);
};

#endif  // AUDIO_H