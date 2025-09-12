#include <AL/al.h>
#include <AL/alc.h>
#include <engine/audio.h>
#include <stdexcept>
#include <fmt/core.h>

ALCdevice* g_device;
ALCcontext* g_context;

static void GetAudioDevices(const ALCchar* devices) {
  const ALCchar* device = devices;
  const ALCchar* next = devices + 1;
  size_t len = 0;
  fmt::print("Available Audio Devices\n");
  while ((device != nullptr) && *device != '\0' && (next != nullptr) && *next != '\0') {
    fmt::print("\t{}\n", device);
    len = strlen(device);
    device += (len + 1);
    next += (len + 2);
  }
}

void audio::Init() {
  ALboolean enumeration = alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT");
  if (enumeration == AL_TRUE) {
    GetAudioDevices(alcGetString(nullptr, ALC_DEVICE_SPECIFIER));
  } else {
    fmt::print("audio: Cannot get devices as AL instance does not support enumeration.\n");
  }
  g_device = alcOpenDevice(nullptr);
  alGetError();  // Clear any errors
  g_context = alcCreateContext(g_device, nullptr);
  if (alcMakeContextCurrent(g_context) == 0) {
    throw std::runtime_error("audio: Failed to make context current");
  }
  fmt::print("Initialized audio\n");
}

void audio::Quit() {
  if (g_context != nullptr) {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(g_context);
    g_context = nullptr;
  }
  if (g_device != nullptr) {
    alcCloseDevice(g_device);
    g_device = nullptr;
  }
  fmt::print("Terminated audio\n");
}
