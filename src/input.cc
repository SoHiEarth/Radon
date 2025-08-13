#include <GLFW/glfw3.h>
#include <engine/global.h>
#include <engine/input.h>
#include <fmt/core.h>
#include <functional>
#include <map>

static const std::map<ButtonState, const char*> kButtonStateMap = {
    {ButtonState::kHold, "Hold"},
    {ButtonState::kPress, "Press"},
    {ButtonState::kRelease, "Release"},
};
std::map<Trigger, std::function<void()>> g_event_hooks;
std::map<int, bool> g_prev_frame_key_states;

void i::AddHook(const Trigger& key, const std::function<void()>& hook) {
  if (glfwGetCurrentContext() == nullptr) {
    fmt::print("No GLFW context to add input hook\n");
  } else {
    g_event_hooks[key] = hook;
    fmt::print("Added input hook for key: {}\tWith state {}\n", key.first,
               kButtonStateMap.at(key.second));
  }
}

void i::RemoveHook(const Trigger& key) {
  if (glfwGetCurrentContext() == nullptr) {
    fmt::print("No GLFW context to remove input hook\n");
    return;
  }
  g_event_hooks.erase(key);
  fmt::print("Removed input hook for key: {}\tWith state {}\n", key.first,
             kButtonStateMap.at(key.second));
}

void i::Init() {
  if (glfwGetCurrentContext() == nullptr) {
    fmt::print("No GLFW context to initialize input\n");
  } else {
    glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    fmt::print("Initialized Input\n");
  }
}

void i::Update() {
  glfwPollEvents();
  for (const auto& [key, hook] : g_event_hooks) {
    bool is_pressed = (glfwGetKey(glfwGetCurrentContext(), key.first) == GLFW_PRESS);
    bool was_pressed = g_prev_frame_key_states[key.first];
    if (key.second == ButtonState::kPress && is_pressed && !was_pressed) {
      hook();
    } else if (key.second == ButtonState::kHold && is_pressed) {
      hook();
    } else if (key.second == ButtonState::kRelease && !is_pressed && was_pressed) {
      hook();
    }
    g_prev_frame_key_states[key.first] = is_pressed;
  }
}

void i::Quit() {
  if (Engine::g_window == nullptr) {
    fmt::print("No GLFW context to quit input\n");
  } else {
    glfwSetInputMode(Engine::g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  fmt::print("Terminated Input\n");
}
