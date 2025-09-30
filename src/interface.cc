#include <engine/interface.h>
#include <format>
#include <fmt/core.h>

std::vector<Interface*>& Interface::All() {
  static std::vector<Interface*> interfaces;
  return interfaces;
}

void Interface::Start() {
  if (state == InterfaceState::Initialized) {
    return;
  }
  bool added = false;
  for (const auto& interface : All()) {
    if (strcmp(interface->name(), name()) == 0) {
      added = true;
    }
  }
  if (!added) {
    Interface::All().push_back(this);
  }
  fmt::print("Starting interface {}", name());
  try {
    i_Init();
  } catch (const std::exception& e) {
    fmt::print("\tFAILED\n");
    fmt::print("Exception during initialization of interface {}: {}\n", name(), e.what());
    state = InterfaceState::Error;
    throw e;
  }
    fmt::print("\tOK\n");
  state = InterfaceState::Initialized;
}

void Interface::Update() {
  if (state != InterfaceState::Initialized) {
    return;
  }
  try {
    i_Update();
  }
  catch (const std::exception& e) {
    fmt::print("Exception during update of interface {}: {}\n", name(), e.what());
    state = InterfaceState::Error;
    throw e;
  }
}

void Interface::Render() {
  if (state != InterfaceState::Initialized) {
    return;
  }
  try {
    i_Render();
  }
  catch (const std::exception& e) {
    fmt::print("Exception during render of interface {}: {}\n", name(), e.what());
    state = InterfaceState::Error;
    throw e;
  }
}

void Interface::Stop() {
  if (state != InterfaceState::Initialized) {
    return;
  }
  fmt::print("Stopping interface {}", name());
  try {
    i_Quit();
  } catch (const std::exception& e) {
    fmt::print("\tFAILED\n");
    fmt::print("Exception during quitting of interface {}: {}\n", name(), e.what());
    state = InterfaceState::Error;
    throw e;
  }
  fmt::print("\tOK\n");
  state = InterfaceState::Stopped;
}