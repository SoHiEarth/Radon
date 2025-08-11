#include <engine/input.h>
#include <GLFW/glfw3.h>
#include <engine/global.h>
#include <fmt/core.h>
#include <map>
#include <functional>
#define GET_KEY_TYPE(key) \
  ((key.second == ButtonState::PRESS) ? "PRESS" : \
   (key.second == ButtonState::HOLD) ? "HOLD" : \
   (key.second == ButtonState::RELEASE) ? "RELEASE" : "UNKNOWN")

std::map<Trigger, std::function<void()>> event_hooks;
std::map<int, bool> prev_frame_key_states;

void i::AddHook(const Trigger& key, const std::function<void()>& hook) {
  if (glfwGetCurrentContext() == nullptr) {
    fmt::print("No GLFW context to add input hook\n");
  } else {
    event_hooks[key] = hook;
    fmt::print("Added input hook for key: {}\tWith state {}\n", key.first, GET_KEY_TYPE(key));
  }
}

void i::RemoveHook(const Trigger& key) {
  if (glfwGetCurrentContext() == nullptr) { 
    fmt::print("No GLFW context to remove input hook\n");
    return;
  }
  event_hooks.erase(key);
  fmt::print("Removed input hook for key: {}\tWith state {}\n", key.first, GET_KEY_TYPE(key));
}

void i::Init() {
  if (glfwGetCurrentContext() == nullptr)
    fmt::print("No GLFW context to initialize input\n");
  else {
    glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    fmt::print("Initialized Input\n");
  }
}

void i::Update() {
  glfwPollEvents();
  for (const auto& [key, hook] : event_hooks) {
    bool is_pressed = (glfwGetKey(glfwGetCurrentContext(), key.first) == GLFW_PRESS);
    bool was_pressed = prev_frame_key_states[key.first];
    if (key.second == ButtonState::PRESS && is_pressed && !was_pressed)
      hook();
    else if (key.second == ButtonState::HOLD && is_pressed)
      hook();
    else if (key.second == ButtonState::RELEASE && !is_pressed && was_pressed)
      hook();
    prev_frame_key_states[key.first] = is_pressed;
  }
}

void i::Quit() {
  if (!Engine::window)
    fmt::print("No GLFW context to quit input\n");
  else
    glfwSetInputMode(Engine::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  fmt::print("Terminated Input\n");
}
