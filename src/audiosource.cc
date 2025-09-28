#include <classes/audiosource.h>
#include <engine/audio.h>

std::string prev_path;

void AudioSource::Init() {
  try {
    handle = audio::Load(path->c_str());
  } catch (std::runtime_error& e) {
    debug::Warning(std::format("Failed to load audio at path {}. {}", *path, e.what()));
  }
  prev_path = *path;
}

void AudioSource::Update() {
  if (*path != prev_path) {
    try {
      audio::Unload(handle);
      handle = audio::Load(path->c_str());
    } catch (std::runtime_error& e) {
      debug::Warning(std::format("Failed to load audio at path {}. {}", *path, e.what()));
    }
    prev_path = *path;
  }
  audio::Play(handle);
}

void AudioSource::Quit() {
  audio::Unload(handle);
}

void AudioSource::Load(pugi::xml_node& node) {
  path = node.attribute("path").as_string();
}

void AudioSource::Save(pugi::xml_node& node) const {
  node.append_attribute("path") = path->c_str();
}