#include <classes/audiosource.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <format>

const char* g_prev_path;

void AudioSource::Init() {
  try {
    handle_ = IAudio::Get<IAudio>().Load(path_);
  } catch (std::runtime_error& e) {
    IDebug::Warning(std::format("Failed to load audio at path {}. {}", *path_, e.what()));
  }
  g_prev_path = path_;
}

void AudioSource::Update() {
  if (strcmp(path_, g_prev_path) == 0) {
    try {
      IAudio::Get<IAudio>().Unload(handle_);
      handle_ = IAudio::Get<IAudio>().Load(path_);
    } catch (std::runtime_error& e) {
      IDebug::Warning(std::format("Failed to load audio at path {}. {}", *path_, e.what()));
    }
    g_prev_path = static_cast<const char*>(path_);
  }
  IAudio::Get<IAudio>().PlaySound(handle_);
}

void AudioSource::Quit() {
  IAudio::Get<IAudio>().Unload(handle_);
}

void AudioSource::Load(pugi::xml_node& node) {
  path_ = node.attribute("path").as_string();
}

void AudioSource::Save(pugi::xml_node& node) const {
  node.append_attribute("path") = (const char*) path_;
}
