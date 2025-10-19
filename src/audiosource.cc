#include <classes/audiosource.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <format>

std::string g_prev_path;

void AudioSource::Init() {
  auto& audio = engine_->GetAudio();
  auto& debug = engine_->GetDebug();
  try {
    handle_ = audio.Load(path_.i_value_);
  } catch (std::runtime_error& e) {
    debug.Warning(std::format("Failed to load audio at path {}. {}", *path_, e.what()));
  }
  g_prev_path = path_;
}

void AudioSource::Update() {
  auto& audio = engine_->GetAudio();
  auto& debug = engine_->GetDebug();
  if (path_ != g_prev_path) {
    try {
      audio.Unload(handle_);
      handle_ = audio.Load(path_.i_value_);
    } catch (std::runtime_error& e) {
      debug.Warning(std::format("Failed to load audio at path {}. {}", *path_, e.what()));
    }
    g_prev_path = path_;
  }
  audio.PlaySound(handle_);
}

void AudioSource::Quit() {
  auto& audio = engine_->GetAudio();
  audio.Unload(handle_);
}

void AudioSource::Load(pugi::xml_node& node) {
  auto& io = engine_->GetIO();
  path_ = io.Load<const char*>(node, "path");
}

void AudioSource::Save(pugi::xml_node& node) const {
  auto& io = engine_->GetIO();
  io.Save(path_.i_value_, node, "path");
}
