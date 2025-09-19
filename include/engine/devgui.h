#ifndef DEVGUI_H
#define DEVGUI_H

enum ConsoleMessageType : std::uint8_t {
  kConsoleMessageTypeInfo = 0,
  kConsoleMessageTypeWarning = 1,
  kConsoleMessageTypeError = 2
};

struct ConsoleMessage {
  std::string traceback_;
  std::string message_;
  ConsoleMessageType type_;
};

namespace dev {
extern bool g_hud_enabled;
void Init();
void Update();
void AddConsoleMessage(const char* traceback, const char* message, std::uint8_t type);
void Render();
void Quit();
}  // namespace dev

#endif  // DEVGUI_H
