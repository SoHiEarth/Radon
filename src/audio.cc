#include <AL/al.h>
#include <AL/alc.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <cstring>
#include <format>
#include <fstream>
#include <map>
#include <vector>

namespace audio {

namespace {
ALCdevice* device = nullptr;
ALCcontext* context = nullptr;

struct SoundData {
  ALuint buffer;
  ALuint source;
  ALsizei frequency;
  bool is_playing = false;
};

std::map<SOUND_HANDLE, SoundData> sounds;
SOUND_HANDLE next = 1;
}  // namespace

void LoadWAV(const char* filename, std::vector<char>& data, ALenum& format, ALsizei& frequency) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    debug::Throw(std::format("Failed to open WAV file: {}", filename));
  }
  char chunk_id[4];
  file.read(chunk_id, 4);
  if (std::strncmp(chunk_id, "RIFF", 4) != 0) {
    debug::Throw(std::format("Invalid WAV file (missing RIFF): {}", filename));
  }
  file.ignore(4);  // Skip chunk size

  char format_id[4];
  file.read(format_id, 4);
  if (std::strncmp(format_id, "WAVE", 4) != 0) {
    debug::Throw(std::format("Invalid WAV file (missing WAVE): {}", filename));
  }

  char subchunk_1_id[4];
  file.read(subchunk_1_id, 4);
  if (std::strncmp(subchunk_1_id, "fmt ", 4) != 0) {
    debug::Throw(std::format("Invalid WAV file (missing fmt ): {}", filename));
  }

  uint32_t subchunk_1_size;
  file.read(reinterpret_cast<char*>(&subchunk_1_size), 4);

  uint16_t audio_format, channels, block_align, bits_per_sample;
  uint32_t sample_rate, byte_rate;
  file.read(reinterpret_cast<char*>(&audio_format), 2);
  file.read(reinterpret_cast<char*>(&channels), 2);
  file.read(reinterpret_cast<char*>(&sample_rate), 4);
  file.read(reinterpret_cast<char*>(&byte_rate), 4);
  file.read(reinterpret_cast<char*>(&block_align), 2);
  file.read(reinterpret_cast<char*>(&bits_per_sample), 2);

  if (audio_format != 1) {
    debug::Throw(std::format("Unsupported WAV audio_format (only PCM supported): {}", filename));
  }
  if (subchunk_1_size > 16) {
    file.ignore(subchunk_1_size - 16);
  }

  char subchunk_2_id[4];
  uint32_t subchunk_2_size = 0;
  while (file.read(subchunk_2_id, 4)) {
    file.read(reinterpret_cast<char*>(&subchunk_2_size), 4);
    if (std::strncmp(subchunk_2_id, "data", 4) == 0) {
      break;
    }
    file.ignore(subchunk_2_size);
  }

  if (subchunk_2_size == 0) {
    debug::Throw(std::format("Invalid WAV file (missing data chunk): {}", filename));
  }

  data.resize(subchunk_2_size);
  file.read(data.data(), subchunk_2_size);

  if (bits_per_sample == 8) {
    format = (channels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
  } else if (bits_per_sample == 16) {
    format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  } else {
    debug::Throw(
        std::format("Unsupported WAV bit depth: {} in file {}", bits_per_sample, filename));
  }

  frequency = sample_rate;
}

void Init() {
  device = alcOpenDevice(nullptr);
  if (!device) {
    debug::Throw("Failed to open OpenAL device.");
  }
  context = alcCreateContext(device, nullptr);
  if (!context || !alcMakeContextCurrent(context)) {
    if (context) {
      alcDestroyContext(context);
    }
    alcCloseDevice(device);
    debug::Throw("Failed to set OpenAL context.");
  }
  debug::Log("Initialized audio");
}

void Update() {
  for (auto& [handle, sound] : sounds) {
    ALint state;
    alGetSourcei(sound.source, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
      alSourcePlay(sound.source);
    }
    sound.is_playing = false;
  }
}

void Quit() {
  for (auto& [handle, sound] : sounds) {
    alDeleteSources(1, &sound.source);
    alDeleteBuffers(1, &sound.buffer);
  }
  sounds.clear();

  alcMakeContextCurrent(nullptr);
  if (context) {
    alcDestroyContext(context);
    context = nullptr;
  }
  if (device) {
    alcCloseDevice(device);
    device = nullptr;
  }
  debug::Log("Quit audio");
}

SOUND_HANDLE Load(const char* filepath) {
  std::vector<char> data;
  ALenum format;
  ALsizei frequency;

  LoadWAV(filepath, data, format, frequency);

  ALuint buffer;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, format, data.data(), static_cast<ALsizei>(data.size()), frequency);
  ALuint source = 0;
  alGenSources(1, &source);
  alSourcei(source, AL_BUFFER, buffer);
  alSourcei(source, AL_LOOPING, AL_FALSE);

  SOUND_HANDLE handle = next++;
  sounds[handle] = {buffer, source};
  return handle;
}

void Unload(SOUND_HANDLE sound) {
  auto it = sounds.find(sound);
  if (it != sounds.end()) {
    alDeleteSources(1, &it->second.source);
    alDeleteBuffers(1, &it->second.buffer);
    sounds.erase(it);
  }
}

void Play(SOUND_HANDLE sound) {
  auto it = sounds.find(sound);
  if (it != sounds.end()) {
    it->second.is_playing = true;
  }
}

void Stop(SOUND_HANDLE sound) {
  auto it = sounds.find(sound);
  if (it != sounds.end()) {
    it->second.is_playing = false;
    alSourceStop(it->second.source);
  }
}

void SetHeader(SOUND_HANDLE sound, float position) {
  auto it = sounds.find(sound);
  if (it != sounds.end()) {
    alSourcef(it->second.source, AL_SEC_OFFSET, position);
  }
}

}  // namespace audio
