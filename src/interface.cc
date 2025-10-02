#include <engine/interface.h>
#include <fmt/core.h>

std::vector<Interface*>& Interface::All() {
  static std::vector<Interface*> interfaces;
  return interfaces;
}

void Interface::Start() {
  if (state_ == InterfaceState::kInitialized) {
    return;
  }
  bool added = false;
  for (const auto& interface : All()) {
    if (strcmp(interface->Name(), Name()) == 0) {
      added = true;
    }
  }
  if (!added) {
    Interface::All().push_back(this);
  }
  fmt::print("Starting interface {}", Name());
  try {
    IInit();
  } catch (const std::exception& e) {
    fmt::print("\tFAILED\n");
    fmt::print("Exception during initialization of interface {}: {}\n", Name(), e.what());
    state_ = InterfaceState::kError;
    throw e;
  }
  fmt::print("\tOK\n");
  state_ = InterfaceState::kInitialized;
}

void Interface::Update() {
  if (state_ != InterfaceState::kInitialized) {
    return;
  }
  try {
    IUpdate();
  } catch (const std::exception& e) {
    fmt::print("Exception during update of interface {}: {}\n", Name(), e.what());
    state_ = InterfaceState::kError;
    throw e;
  }
}

void Interface::Render() {
  if (state_ != InterfaceState::kInitialized) {
    return;
  }
  try {
    IRender();
  } catch (const std::exception& e) {
    fmt::print("Exception during render of interface {}: {}\n", Name(), e.what());
    state_ = InterfaceState::kError;
    throw e;
  }
}

void Interface::Stop() {
  if (state_ != InterfaceState::kInitialized) {
    return;
  }
  fmt::print("Stopping interface {}", Name());
  try {
    IQuit();
  } catch (const std::exception& e) {
    fmt::print("\tFAILED\n");
    fmt::print("Exception during quitting of interface {}: {}\n", Name(), e.what());
    state_ = InterfaceState::kError;
    throw e;
  }
  fmt::print("\tOK\n");
  state_ = InterfaceState::kStopped;
}
