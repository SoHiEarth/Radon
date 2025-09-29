#include <classes/audiosource.h>
#include <engine/audio.h>

std::string prev_path;

void AudioSource::Init() {
  try {
    handle = IAudio::Get<IAudio>().Load(path->c_str());
  } catch (std::runtime_error& e) {
    IDebug::Warning(std::format("Failed to load audio at path {}. {}", *path, e.what()));
  }
  prev_path = *path;
}

void AudioSource::Update() {
  if (*path != prev_path) {
    try {
      IAudio::Get<IAudio>().Unload(handle);
      handle = IAudio::Get<IAudio>().Load(path->c_str());
    } catch (std::runtime_error& e) {
      IDebug::Warning(std::format("Failed to load audio at path {}. {}", *path, e.what()));
    }
    prev_path = *path;
  }
  IAudio::Get<IAudio>().PlaySound(handle);
}

void AudioSource::Quit() {
  IAudio::Get<IAudio>().Unload(handle);
}

void AudioSource::Load(pugi::xml_node& node) {
  path = node.attribute("path").as_string();
}

void AudioSource::Save(pugi::xml_node& node) const {
  node.append_attribute("path") = path->c_str();
}