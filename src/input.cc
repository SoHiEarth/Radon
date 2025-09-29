#include <GLFW/glfw3.h>
#include <engine/debug.h>
#include <engine/input.h>
#include <engine/render.h>
#include <format>
#include <functional>
#include <map>

static const std::map<ButtonState, std::string> kButtonStateMap {
    {ButtonState::kHold, "Hold"},
    {ButtonState::kPress, "Press"},
    {ButtonState::kRelease, "Release"},
};
std::map<Trigger, std::function<void()>> g_event_hooks;
std::map<int, bool> g_prev_frame_key_states;

void IInput::AddHook(const Trigger& key, const std::function<void()>& hook) {
  if (glfwGetCurrentContext() == nullptr) {
    IDebug::Throw("No GLFW context to add input hook");
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

void IInput::i_Init() {
  if (glfwGetCurrentContext() == nullptr) {
    throw std::runtime_error("No GLFW context to initialize input");
  }
  glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void IInput::i_Update() {
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

void IInput::i_Quit() {
  if (IRenderer::Get<IRenderer>().GetWindow() == nullptr) {
    IDebug::Throw("No GLFW context to quit input");
  } else {
    glfwSetInputMode(IRenderer::Get<IRenderer>().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}
