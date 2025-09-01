#include <AL/al.h>
#include <AL/alc.h>
#include <engine/audio.h>
#include <cstdio>
#include <cstring>

ALCdevice* g_device;
ALCcontext* g_context;

static void GetAudioDevices(const ALCchar* devices) {
  const ALCchar* device = devices;
  const ALCchar* next = devices + 1;
  size_t len = 0;
  printf("Available Audio Devices\n");
  while ((device != nullptr) && *device != '\0' && (next != nullptr) && *next != '\0') {
    printf("%s\n", device);
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
    printf("AL: Cannot get devices as AL instance does not support enumeration.\n");
  }
  g_device = alcOpenDevice(nullptr);
  alGetError();
  g_context = alcCreateContext(g_device, nullptr);
  if (alcMakeContextCurrent(g_context) == 0) {
    printf("AL: Failed to make context current");
    return;
  }
  printf("Initialized OpenAL\n");
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
  printf("Terminated OpenAL\n");
}
