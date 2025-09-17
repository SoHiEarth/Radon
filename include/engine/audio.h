#ifndef AUDIO_H
#define AUDIO_H

#define SOUND_HANDLE unsigned int

namespace audio {
void Init();
// Called every frame to process audio.
void Update();
void Quit();

SOUND_HANDLE Load(const char* filepath);
void Unload(SOUND_HANDLE sound);
// Schedules the sound to be played on the next update, until stopped with audio::Stop
void Play(SOUND_HANDLE sound);
// Stops the sound if it is scheduled to play.
void Stop(SOUND_HANDLE sound);
// Sets the time position of the sound in seconds.
void SetHeader(SOUND_HANDLE sound, float position);

};  // namespace audio

#endif  // AUDIO_H
