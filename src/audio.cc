#include <AL/al.h>
#include <AL/alc.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <engine/engine.h>
#include <cstring>
#include <format>
#include <fstream>
#include <map>
#include <vector>

void IAudio::LoadWAV(const char* filename, std::vector<char>& data, ALenum& format,
                     ALsizei& frequency) {
  auto& debug = engine_->GetDebug();
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    debug.Throw(std::format("Failed to open WAV file: {}", filename));
  }
  std::array<char, 4> chunk_id;
  std::array<char, 4> format_id;
  std::array<char, 4> subchunk_1_id;
  file.read(chunk_id.data(), 4);
  if (std::strncmp(chunk_id.data(), "RIFF", 4) != 0) {
    debug.Throw(std::format("Invalid WAV file (missing RIFF): {}", filename));
  }
  file.ignore(4);  // Skip chunk size

  file.read(format_id.data(), 4);
  if (std::strncmp(format_id.data(), "WAVE", 4) != 0) {
    debug.Throw(std::format("Invalid WAV file (missing WAVE): {}", filename));
  }

  file.read(subchunk_1_id.data(), 4);
  if (std::strncmp(subchunk_1_id.data(), "fmt ", 4) != 0) {
    debug.Throw(std::format("Invalid WAV file (missing fmt ): {}", filename));
  }

  uint32_t subchunk_1_size = 0;
  file.read(reinterpret_cast<char*>(&subchunk_1_size), 4);

  uint16_t audio_format = 0;
  uint16_t channels = 0;
  uint16_t block_align = 0;
  uint16_t bits_per_sample = 0;
  uint32_t sample_rate = 0;
  uint32_t byte_rate = 0;
  file.read(reinterpret_cast<char*>(&audio_format), 2);
  file.read(reinterpret_cast<char*>(&channels), 2);
  file.read(reinterpret_cast<char*>(&sample_rate), 4);
  file.read(reinterpret_cast<char*>(&byte_rate), 4);
  file.read(reinterpret_cast<char*>(&block_align), 2);
  file.read(reinterpret_cast<char*>(&bits_per_sample), 2);

  if (audio_format != 1) {
    debug.Throw(std::format("Unsupported WAV audio_format (only PCM supported): {}", filename));
  }
  if (subchunk_1_size > 16) {
    file.ignore(static_cast<std::streamsize>(subchunk_1_size) - 16);
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
    debug.Throw(std::format("Invalid WAV file (missing data chunk): {}", filename));
  }

  data.resize(subchunk_2_size);
  file.read(data.data(), subchunk_2_size);

  if (bits_per_sample == 8) {
    format = (channels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
  } else if (bits_per_sample == 16) {
    format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  } else {
    debug.Throw(std::format("Unsupported WAV bit depth: {} in file {}", bits_per_sample, filename));
  }

  frequency = sample_rate;
}

void IAudio::IInit() {
  auto& debug = engine_->GetDebug();
  g_device_ = alcOpenDevice(nullptr);
  if (g_device_ == nullptr) {
    debug.Throw("Failed to open OpenAL device.");
  }
  g_context_ = alcCreateContext(g_device_, nullptr);
  if ((g_context_ == nullptr) || (alcMakeContextCurrent(g_context_) == 0)) {
    if (g_context_ != nullptr) {
      alcDestroyContext(g_context_);
    }
    alcCloseDevice(g_device_);
    debug.Throw("Failed to set OpenAL context.");
  }
}

void IAudio::IUpdate() {
  for (auto& [handle, sound] : g_sounds_) {
    ALint state;
    alGetSourcei(sound.source_, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING) {
      alSourcePlay(sound.source_);
    }
    sound.is_playing_ = false;
  }
}

void IAudio::IQuit() {
  for (auto& [handle, sound] : g_sounds_) {
    alDeleteSources(1, &sound.source_);
    alDeleteBuffers(1, &sound.buffer_);
  }
  g_sounds_.clear();

  alcMakeContextCurrent(nullptr);
  if (g_context_ != nullptr) {
    alcDestroyContext(g_context_);
    g_context_ = nullptr;
  }
  if (g_device_ != nullptr) {
    alcCloseDevice(g_device_);
    g_device_ = nullptr;
  }
}

unsigned int IAudio::Load(std::string_view filepath) {
  std::vector<char> data;
  ALenum format;
  ALsizei frequency;

  LoadWAV(filepath.data(), data, format, frequency);

  ALuint buffer;
  alGenBuffers(1, &buffer);
  alBufferData(buffer, format, data.data(), static_cast<ALsizei>(data.size()), frequency);
  ALuint source = 0;
  alGenSources(1, &source);
  alSourcei(source, AL_BUFFER, buffer);
  alSourcei(source, AL_LOOPING, AL_FALSE);

  unsigned int handle = g_next_++;
  g_sounds_[handle] = {.buffer_ = buffer, .source_ = source};
  return handle;
}

void IAudio::Unload(unsigned int sound) {
  auto it = g_sounds_.find(sound);
  if (it != g_sounds_.end()) {
    alDeleteSources(1, &it->second.source_);
    alDeleteBuffers(1, &it->second.buffer_);
    g_sounds_.erase(it);
  }
}

void IAudio::PlaySound(unsigned int sound) {
  auto it = g_sounds_.find(sound);
  if (it != g_sounds_.end()) {
    it->second.is_playing_ = true;
  }
}

void IAudio::StopSound(unsigned int sound) {
  auto it = g_sounds_.find(sound);
  if (it != g_sounds_.end()) {
    it->second.is_playing_ = false;
    alSourceStop(it->second.source_);
  }
}

void IAudio::SetHeader(unsigned int sound, float position) {
  auto it = g_sounds_.find(sound);
  if (it != g_sounds_.end()) {
    alSourcef(it->second.source_, AL_SEC_OFFSET, position);
  }
}
