#include <GLFW/glfw3.h>
#include <classes/level.h>
#include <classes/object.h>
#include <engine/debug.h>
#include <algorithm>
#include <format>

void Level::Init() {
  glfwSetWindowTitle(glfwGetCurrentContext(), std::format("Radon Engine - {}", path_).c_str());
  for (const auto& object : objects_) {
    if (object != nullptr) {
      if (!object->has_initialized_ && !object->has_quit_) {
        object->Init();
      }
    }
  }
}

void Level::Update() {
  for (const auto& object : objects_) {
    if (object != nullptr) {
      if (object->has_initialized_ && !object->has_quit_) {
        object->Update();
      }
    }
  }
}

void Level::Render() {
  for (const auto& object : objects_) {
    if (object != nullptr) {
      if (object->has_initialized_ && !object->has_quit_) {
        object->Render();
      }
    }
  }
}

void Level::Quit() {
  for (const auto& object : objects_) {
    if (object != nullptr) {
      if (object->has_initialized_ && !object->has_quit_) {
        object->Quit();
      }
    }
  }
}

void Level::AddObject(Object* object, std::string_view name) {
  if (object == nullptr) {
    return;
  }
  if (name != "Object") {
    object->name_ = std::string(name);
  }
  if (!object->has_initialized_) {
    debug::Log(std::format("Initializing object: {}", *object->name_));
    object->Init();
    object->has_initialized_ = true;
  }
  objects_.push_back(object);
}

void Level::RemoveObject(const Object* object) {
  if (object == nullptr) {
    return;
  }

  auto it = std::ranges::find(objects_.begin(), objects_.end(), object);
  if (it != objects_.end()) {
    if (!(*it)->has_quit_ && (*it)->has_initialized_) {
      (*it)->Quit();
      (*it)->has_quit_ = true;
    }
    objects_.erase(it);
  }
}
