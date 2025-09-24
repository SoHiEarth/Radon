#include <classes/object.h>
#include <classes/component.h>
#include <format>
#include <engine/telemetry.h>
#include <engine/io.h>
#include <engine/debug.h>

void Object::Init() {
  if (has_initialized_) return;
  auto timer_code = std::format("Object Init {}", name_.i_value_);
  telemetry::BeginTimer(timer_code.c_str());
  for (const auto& component : components_) {
    component->Init();
  }
  has_initialized_ = true;
  telemetry::EndTimer(timer_code.c_str());
}

void Object::Update() {
  if (!has_initialized_) {
    Init();
  }
  auto timer_code = std::format("Object Update {}", name_.i_value_);
  telemetry::BeginTimer(timer_code.c_str());
  for (const auto& component : components_) {
    component->Update();
  }
  telemetry::EndTimer(timer_code.c_str());
}

void Object::Render() {
  if (!has_initialized_) {
    Init();
  }
  auto timer_code = std::format("Object Render {}", name_.i_value_);
  telemetry::BeginTimer(timer_code.c_str());
  for (const auto& component : components_) {
    component->Render();
  }
  telemetry::EndTimer(timer_code.c_str());
}

void Object::Quit() {
  if (has_quit_) return;
  auto timer_code = std::format("Object Quit {}", name_.i_value_);
  telemetry::BeginTimer(timer_code.c_str());
  for (const auto& component : components_) {
    component->Quit();
  }
  has_quit_ = true;
  telemetry::EndTimer(timer_code.c_str());
}

void Object::Load(pugi::xml_node& node) {
  name_ = io::xml::LoadString(node, name_.i_label_);
  position_ = io::xml::LoadVec3(node, position_.i_label_);
  rotation_ = io::xml::LoadVec3(node, rotation_.i_label_);
  scale_ = io::xml::LoadVec2(node, scale_.i_label_);
  is_static_ = io::xml::LoadInt(node, is_static_.i_label_);
  for (auto& component : components_) {
    if (!component)
      continue;
    component->parent_ = shared_from_this();
    component->Load(node);
  }
}

void Object::Save(pugi::xml_node& node) const {
  int is_static_int = static_cast<int>(is_static_.i_value_);
  io::xml::SaveInt(&is_static_int, node, is_static_.i_label_);
  io::xml::SaveString(name_.i_value_, node, name_.i_label_);
  io::xml::SaveVec3(position_.i_value_, node, position_.i_label_);
  io::xml::SaveVec2(scale_.i_value_, node, scale_.i_label_);
  io::xml::SaveVec3(rotation_.i_value_, node, rotation_.i_label_);
  for (const auto& component : components_) {
    component->Save(node);
  }
}

void Object::AddComponent(std::unique_ptr<Component> component) {
  if (component == nullptr) {
    return;
  }
  for (const auto& existing_component : components_) {
    if (existing_component != nullptr && existing_component->GetTypeName() == component->GetTypeName()) {
      debug::Throw(std::format("Object already has a component of type {}.", component->GetTypeName()));
    }
  }
  component->parent_ = shared_from_this();
  components_.push_back(std::move(component));
}

void Object::RemoveComponent(std::weak_ptr<Component> component) {
  if (auto com = component.lock()) {
    auto it = std::find_if(
        components_.begin(), components_.end(),
        [&](const std::unique_ptr<Component>& uptr) { return uptr.get() == com.get(); });
    if (it != components_.end()) {
      if ((*it)->has_initialized_ && !(*it)->has_quit_) {
        (*it)->Quit();
        (*it)->has_quit_ = true;
      }
      components_.erase(it);
    }
  }
}