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
  for (Component* component : components_) {
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
  for (Component* component : components_) {
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
  for (Component* component : components_) {
    component->Render();
  }
  telemetry::EndTimer(timer_code.c_str());
}

void Object::Quit() {
  if (has_quit_) return;
  auto timer_code = std::format("Object Quit {}", name_.i_value_);
  telemetry::BeginTimer(timer_code.c_str());
  for (Component* component : components_) {
    component->Quit();
  }
  has_quit_ = true;
  telemetry::EndTimer(timer_code.c_str());
}

void Object::Load(pugi::xml_node& node) {
  name_ = io::serialized::LoadString(node, name_.i_label_);
  position_ = io::serialized::LoadVec3(node, position_.i_label_);
  rotation_ = io::serialized::LoadVec3(node, rotation_.i_label_);
  scale_ = io::serialized::LoadVec2(node, scale_.i_label_);
  is_static_ = io::serialized::LoadInt(node, is_static_.i_label_);
  for (Component* component : components_) {
    if (!component)
      continue;
    component->parent_ = this;
    component->Load(node);
  }
}

void Object::Save(pugi::xml_node& node) const {
  int is_static_int = static_cast<int>(is_static_.i_value_);
  io::serialized::SaveInt(&is_static_int, node, is_static_.i_label_);
  io::serialized::SaveString(&name_.i_value_, node, name_.i_label_);
  io::serialized::SaveVec3(&position_.i_value_, node, position_.i_label_);
  io::serialized::SaveVec2(&scale_.i_value_, node, scale_.i_label_);
  io::serialized::SaveVec3(&rotation_.i_value_, node, rotation_.i_label_);
  for (const auto& component : components_) {
    component->Save(node);
  }
}

void Object::AddComponent(Component* component) {
  if (component == nullptr) {
    return;
  }
  for (const auto& existing_component : components_) {
    if (existing_component != nullptr && existing_component->GetTypeName() == component->GetTypeName()) {
      debug::Throw(std::format("Object already has a component of type {}.", component->GetTypeName()));
    }
  }
  component->parent_ = this;
  components_.push_back(component);
}

void Object::RemoveComponent(Component* component) {
  if (component == nullptr) {
    return;
  }
  auto it = std::find(components_.begin(), components_.end(), component);
  if (it != components_.end()) {
    if (component->has_initialized_ && !component->has_quit_) {
      component->Quit();
      component->has_quit_ = true;
    }
    components_.erase(it);
    delete component;
  }
}