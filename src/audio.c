#include <AL/al.h>
#include <AL/alc.h>
#include <engine/audio.h>
#include <stdio.h>
#include <string.h>

ALCdevice* g_device;
ALCcontext* g_context;

static void GetAudioDevices(const ALCchar* devices) {
  const ALCchar* device = devices;
  const ALCchar* next = devices + 1;
  size_t len = 0;
  printf("Available Audio Devices\n");
  while ((device != NULL) && *device != '\0' && (next != NULL) && *next != '\0') {
    printf("%s\n", device);
    len = strlen(device);
    device += (len + 1);
    next += (len + 2);
  }
}

void AInit() {
  ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
  if (enumeration == AL_TRUE) {
    GetAudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
  } else {
    printf("AL: Cannot get devices as AL instance does not support enumeration.\n");
  }
  g_device = alcOpenDevice(NULL);
  alGetError();
  g_context = alcCreateContext(g_device, NULL);
  if (alcMakeContextCurrent(g_context) == 0) {
    printf("AL: Failed to make context current");
    return;
  }
  printf("Initialized OpenAL\n");
}

void AQuit() {
  if (g_context != NULL) {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(g_context);
    g_context = NULL;
  }
  if (g_device != NULL) {
    alcCloseDevice(g_device);
    g_device = NULL;
  }
  printf("Terminated OpenAL\n");
}
