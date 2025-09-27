#ifndef AUDIO_H
#define AUDIO_H

using SoundHandle = unsigned int;

namespace audio {
void Init();
void Update();
void Quit();

SoundHandle Load(const char* /*filepath*/);
void Unload(SoundHandle /*sound*/);

// Schedules the sound to be played on the next update, until stopped with audio::Stop
void Play(SoundHandle /*sound*/);
// Stops the sound if it is scheduled to play.
void Stop(SoundHandle /*sound*/);
// Sets the time position of the sound in seconds.
void SetHeader(SoundHandle /*sound*/, float /*position*/);

};  // namespace audio

#endif  // AUDIO_H
