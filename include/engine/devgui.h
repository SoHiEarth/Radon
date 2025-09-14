#ifndef DEVGUI_H
#define DEVGUI_H

enum ConsoleMessageType : std::uint8_t {
  ConsoleMessageType_Info = 0,
  ConsoleMessageType_Warning = 1,
  ConsoleMessageType_Error = 2
};

struct ConsoleMessage {
#ifndef MDEBUG_DISABLE_TRACE
  std::string traceback_;
#endif
  std::string message_;
  ConsoleMessageType type_;
};

namespace dev {
extern bool g_hud_enabled;
void Init();
void Update();
#ifdef MDEBUG_DISABLE_TRACE
void AddConsoleMessage(const char* message, std::uint8_t type);
#else
void AddConsoleMessage(const char* traceback, const char* message, std::uint8_t type);
#endif
void Render();
void Quit();
}  // namespace dev

#endif  // DEVGUI_H
