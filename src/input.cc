#include <engine/input.h>
#include <GLFW/glfw3.h>
#include <engine/global.h>
#include <fmt/core.h>
#include <map>
#include <functional>
#define GET_KEY_TYPE(key) \
  ((key.second == ButtonState::BUTTON_STATE_PRESS) ? "PRESS" : \
   (key.second == ButtonState::BUTTON_STATE_HOLD) ? "HOLD" : \
   (key.second == ButtonState::BUTTON_STATE_RELEASE) ? "RELEASE" : "UNKNOWN")

std::map<std::pair<int, ButtonState>, std::function<void()>> event_hooks;
std::map<int, bool> prev_frame_key_states;

void i::AddHook(std::pair<int, ButtonState> key, std::function<void()> hook) {
  if (glfwGetCurrentContext() == nullptr) {
    fmt::print("No GLFW context to add input hook\n");
  } else {
    event_hooks[key] = hook;
    fmt::print("Added input hook for key: {}\tWith state {}\n", key.first, GET_KEY_TYPE(key));
  }
}

void i::RemoveHook(std::pair<int, ButtonState> key) {
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
  for (const auto& [entry, hook] : event_hooks) {
    bool is_pressed = glfwGetKey(glfwGetCurrentContext(), entry.first) == GLFW_PRESS;
    bool was_pressed = prev_frame_key_states[entry.first];
    if (entry.second == ButtonState::BUTTON_STATE_PRESS && is_pressed && !was_pressed) {
      hook();
    } else if (entry.second == ButtonState::BUTTON_STATE_HOLD && is_pressed) {
      hook();
    } else if (entry.second == ButtonState::BUTTON_STATE_RELEASE && !is_pressed && was_pressed) {
      hook();
    }
    prev_frame_key_states[entry.first] = is_pressed;
  }
}

void i::Quit() {
  if (!Engine::window.load())
    fmt::print("No GLFW context to quit input\n");
  else
    glfwSetInputMode(Engine::window.load(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  fmt::print("Terminated Input\n");
}
