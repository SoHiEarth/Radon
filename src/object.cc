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
  ITelemetry::Get<ITelemetry>().BeginTimer(timer_code.c_str());
  for (const auto& component : components_) {
    component->Init();
  }
  has_initialized_ = true;
  ITelemetry::Get<ITelemetry>().EndTimer(timer_code.c_str());
}

void Object::Update() {
  if (!has_initialized_) {
    Init();
  }
  auto timer_code = std::format("Object Update {}", name_.i_value_);
  ITelemetry::Get<ITelemetry>().BeginTimer(timer_code.c_str());
  for (const auto& component : components_) {
    component->Update();
  }
  ITelemetry::Get<ITelemetry>().EndTimer(timer_code.c_str());
}

void Object::Render() {
  if (!has_initialized_) {
    Init();
  }
  auto timer_code = std::format("Object Render {}", name_.i_value_);
  ITelemetry::Get<ITelemetry>().BeginTimer(timer_code.c_str());
  for (const auto& component : components_) {
    component->Render();
  }
  ITelemetry::Get<ITelemetry>().EndTimer(timer_code.c_str());
}

void Object::Quit() {
  if (has_quit_) {
    return;
  }
  auto timer_code = std::format("Object Quit {}", name_.i_value_);
  ITelemetry::Get<ITelemetry>().BeginTimer(timer_code.c_str());
  for (const auto& component : components_) {
    component->Quit();
  }
  has_quit_ = true;
  ITelemetry::Get<ITelemetry>().EndTimer(timer_code.c_str());
}

void Object::Load(pugi::xml_node& node) {
  name_ = IIO::Get<IIO>().LoadString(node, name_.i_label_).c_str();
  for (auto& component : components_) {
    component->parent_ = this;
    component->Load(node);
  }
}

void Object::Save(pugi::xml_node& node) const {
  IIO::Get<IIO>().SaveString(name_.i_value_, node, name_.i_label_);
  for (const auto& component : components_) {
    component->Save(node);
  }
}

void Object::AddComponent(Component* component) {
  if (component == nullptr) {
    return;
  }
  for (const auto& existing_component : components_) {
    if (existing_component != nullptr &&
        existing_component->GetTypeName() == component->GetTypeName()) {
      IDebug::Throw(
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

void Object::RemoveComponent(Component* component) {
  components_.erase(
      std::remove_if(components_.begin(), components_.end(),
                     [&](const Component* c) { return c == component; }),
      components_.end());
}
