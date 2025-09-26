#include <classes/component.h>
#include <classes/object.h>
#include <engine/debug.h>
#include <engine/io.h>
#include <engine/telemetry.h>
#include <format>

void Object::Init() {
  if (has_initialized_) {
    return;
  }
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
  if (has_quit_) {
    return;
  }
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
  for (auto& component : components_) {
    component->parent_ = shared_from_this();
    component->Load(node);
  }
}

void Object::Save(pugi::xml_node& node) const {
  io::xml::SaveString(name_.i_value_, node, name_.i_label_);
  for (const auto& component : components_) {
    component->Save(node);
  }
}

void Object::AddComponent(std::shared_ptr<Component> component) {
  if (component == nullptr) {
    return;
  }
  for (const auto& existing_component : components_) {
    if (existing_component != nullptr &&
        existing_component->GetTypeName() == component->GetTypeName()) {
      debug::Throw(
          std::format("Object already has a component of type {}.", component->GetTypeName()));
    }
  }
  component->parent_ = shared_from_this();
  components_.push_back(std::move(component));
  if (has_initialized_) {
    components_.back()->Init();
    components_.back()->has_initialized_ = true;
  }
}

void Object::RemoveComponent(std::shared_ptr<Component> component) {
  components_.erase(
      std::remove_if(components_.begin(), components_.end(),
                     [&](const std::shared_ptr<Component>& c) { return c == component; }),
      components_.end());
}
