#include <GLFW/glfw3.h>
#include <classes/level.h>
#include <classes/object.h>
#include <engine/debug.h>
#include <algorithm>
#include <format>
#include <engine/job_system.h>

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
  auto& job_system = engine_->GetJobSystem();
  std::vector<JobHandle> handles;

  for (const auto& object : objects_) {
    if (object != nullptr && object->has_initialized_ && !object->has_quit_) {
      handles.push_back(job_system.Schedule([&object]() { object->Update(); }, Priority::kHigh));
    }
  }
  engine_->GetJobSystem().WaitForAll();
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

Object* Level::NewObject(const std::string& name) {
  Object* object = new Object(engine_);
  object->name_ = name;
  if (!object->has_initialized_) {
    object->Init();
    object->has_initialized_ = true;
  }
  objects_.emplace_back(object);
  return object;
}

void Level::RemoveObject(const Object* object) {
  if (object == nullptr) {
    return;
  }

  auto it = std::ranges::find_if(objects_.begin(), objects_.end(),
                                 [object](const std::unique_ptr<Object>& obj) {
                                   return obj.get() == object;  // Compare raw pointers
                                 });

  if (it != objects_.end()) {
    if (!(*it)->has_quit_ && (*it)->has_initialized_) {
      (*it)->Quit();
      (*it)->has_quit_ = true;
    }
    objects_.erase(it);
  }
}

void Level::RemoveObject(const std::unique_ptr<Object>& object) {
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
