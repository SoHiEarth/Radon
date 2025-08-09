#include <engine/audio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <fmt/core.h>

ALCdevice* device;
ALCcontext* context;

void a::Init() {
  device = alcOpenDevice(nullptr);
  if (!device) {
    return;
  }

  context = alcCreateContext(device, nullptr);
  if (!context) {
    alcCloseDevice(device);
    return;
  }

  alcMakeContextCurrent(context);

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
