#include <engine/interface.h>
#include <format>
#include <fmt/core.h>
void Interface::Start() {
  fmt::print("Starting interface {}", name());
  Init();
  fmt::print("\tOK\n");
}

void Interface::Stop() {
  fmt::print("Stopping interface {}", name());
  Quit();
  fmt::print("\tOK\n");
}