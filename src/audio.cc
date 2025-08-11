#include <engine/audio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <fmt/core.h>

ALCdevice* device;
ALCcontext* context;

static void GetAudioDevices(const ALCchar* devices) {
  const ALCchar* device = devices, *next = devices + 1;
  size_t len = 0;
  fmt::print("Available Audio Devices\n");
  while (device && *device != '\0' && next && *next != '\0') {
    fmt::print("{}\n", device);
    len = strlen(device);
    device += (len + 1);
    next += (len + 2);
  }
}

void a::Init() {
  ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
  if (enumeration == AL_TRUE)
    GetAudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
  else
    fmt::print("AL: Cannot get devices as AL instance does not support enumeration.\n");
  device = alcOpenDevice(nullptr);
  alGetError();
  context = alcCreateContext(device, NULL);
  if (!alcMakeContextCurrent(context)) {
    fmt::print("AL: Failed to make context current\n");
    return;
  }
  fmt::print("Initialized OpenAL\n");
}

void a::Quit() {
  if (context) {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    context = nullptr;
  }
  if (device) {
    alcCloseDevice(device);
    device = nullptr;
  }
  fmt::print("Terminated OpenAL\n");
}
