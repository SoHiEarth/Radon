#include <engine/interface.h>
#include <format>
#include <fmt/core.h>

std::vector<Interface*>& Interface::All() {
  static std::vector<Interface*> interfaces;
  return interfaces;
}

void Interface::Start() {
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
  i_Init();
  fmt::print("\tOK\n");
  started = true;
}

void Interface::Update() {
  if (!started) {
    return;
  }
  i_Update();
}

void Interface::Render() {
  if (!started) {
    return;
  }
  i_Render();
}

void Interface::Stop() {
  if (!started) {
    return;
  }
  fmt::print("Stopping interface {}", name());
  i_Quit();
  fmt::print("\tOK\n");
  started = false;
}