#include <GLFW/glfw3.h>
#include <engine/debug.h>
#include <engine/input.h>
#include <engine/render.h>
#include <format>
#include <functional>
#include <map>

static const std::map<ButtonState, std::string> kButtonStateMap{
    {ButtonState::kHold, "Hold"},
    {ButtonState::kPress, "Press"},
    {ButtonState::kRelease, "Release"},
};
std::map<Trigger, std::function<void(Engine*)>> g_event_hooks;
std::map<int, bool> g_prev_frame_key_states;

void IInput::AddHook(const Trigger& key, const std::function<void(Engine*)>& hook) {
  if (glfwGetCurrentContext() == nullptr) {
    engine_->GetDebug().Throw("No GLFW context to add input hook");
  } else {
    g_event_hooks[key] = hook;
  }
}

void IInput::RemoveHook(const Trigger& key) {
  if (glfwGetCurrentContext() == nullptr) {
    throw std::runtime_error("No GLFW context to remove input hook");
    return;
  }
  g_event_hooks.erase(key);
}

void IInput::IInit() {
  if (glfwGetCurrentContext() == nullptr) {
    throw std::runtime_error("No GLFW context to initialize input");
  }
  glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void IInput::IUpdate() {
  glfwPollEvents();
  for (const auto& [key, hook] : g_event_hooks) {
    bool is_pressed = (glfwGetKey(glfwGetCurrentContext(), key.first) == GLFW_PRESS);
    bool was_pressed = g_prev_frame_key_states[key.first];
    if (key.second == ButtonState::kPress && is_pressed && !was_pressed) {
      hook(engine_);
    } else if (key.second == ButtonState::kHold && is_pressed) {
      hook(engine_);
    } else if (key.second == ButtonState::kRelease && !is_pressed && was_pressed) {
      hook(engine_);
    }
    g_prev_frame_key_states[key.first] = is_pressed;
  }
}

void IInput::IQuit() {
  if (engine_->GetRenderer().GetWindow() == nullptr) {
    engine_->GetDebug().Throw("No GLFW context to quit input");
  } else {
    glfwSetInputMode(engine_->GetRenderer().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}
