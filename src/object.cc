#include <classes/component.h>
#include <classes/object.h>
#include <engine/debug.h>
#include <engine/io.h>
#include <engine/telemetry.h>
#include <algorithm>
#include <format>

void Object::Init() {
  if (has_initialized_) {
    return;
  }
  for (const auto& component : components_) {
    component->Init();
  }
  has_initialized_ = true;
}

void Object::Update() {
  if (!has_initialized_) {
    Init();
  }
  for (const auto& component : components_) {
    component->Update();
  }
}

void Object::Render() {
  if (!has_initialized_) {
    Init();
  }
  for (const auto& component : components_) {
    component->Render();
  }
}

void Object::Quit() {
  if (has_quit_) {
    return;
  }
  for (const auto& component : components_) {
    component->Quit();
  }
  has_quit_ = true;
}

void Object::Load(pugi::xml_node& node) {
  name_ = IIO::Load<std::string>(node, name_.i_label_).c_str();
  for (auto& component : components_) {
    component->parent_ = this;
    component->Load(node);
  }
}

void Object::Save(pugi::xml_node& node) const {
  IIO::Save<std::string>(name_.i_value_, node, name_.i_label_);
  for (const auto& component : components_) {
    component->Save(node);
  }
}

void Object::AddComponent(std::unique_ptr<Component>& component) {
  if (component == nullptr) {
    return;
  }
  for (const auto& existing_component : components_) {
    if (existing_component != nullptr &&
        existing_component->GetTypeName() == component->GetTypeName()) {
      engine_->GetDebug().Throw(
          std::format("Object already has a component of type {}.", component->GetTypeName()));
    }
  }
  component->parent_ = this;
  components_.push_back(std::move(component));
  if (has_initialized_) {
    components_.back()->Init();
    components_.back()->has_initialized_ = true;
  }
}

void Object::RemoveComponent(const std::unique_ptr<Component>& component) {
  components_.erase(std::remove(components_.begin(), components_.end(), component),
                    components_.end());
}
