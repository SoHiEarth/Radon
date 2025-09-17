#include <GLFW/glfw3.h>
#include <engine/debug.h>
#include <engine/input.h>
#include <engine/render.h>
#include <format>
#include <functional>
#include <map>

static const std::map<ButtonState, const char*> kButtonStateMap = {
    {ButtonState::kHold, "Hold"},
    {ButtonState::kPress, "Press"},
    {ButtonState::kRelease, "Release"},
};
std::map<Trigger, std::function<void()>> g_event_hooks;
std::map<int, bool> g_prev_frame_key_states;

void input::AddHook(const Trigger& key, const std::function<void()>& hook) {
  if (glfwGetCurrentContext() == nullptr) {
    debug::Throw(GET_TRACE, "No GLFW context to add input hook");
  } else {
    g_event_hooks[key] = hook;
    debug::Log(GET_TRACE, std::format("Added input hook for key: {}\tWith state {}", key.first,
                                      kButtonStateMap.at(key.second)));
  }
}

void input::RemoveHook(const Trigger& key) {
  if (glfwGetCurrentContext() == nullptr) {
    throw std::runtime_error("No GLFW context to remove input hook");
    return;
  }
  g_event_hooks.erase(key);
  debug::Log(GET_TRACE, std::format("Removed input hook for key: {}\tWith state {}", key.first,
                                    kButtonStateMap.at(key.second)));
}

void input::Init() {
  if (glfwGetCurrentContext() == nullptr) {
    throw std::runtime_error("No GLFW context to initialize input");
  }
  glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  debug::Log(GET_TRACE, "Initialized input");
}

void input::Update() {
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

void input::Quit() {
  if (render::g_window == nullptr) {
    debug::Throw(GET_TRACE, "No GLFW context to quit input");
  } else {
    glfwSetInputMode(render::g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
  debug::Log(GET_TRACE, "Quit input");
}
