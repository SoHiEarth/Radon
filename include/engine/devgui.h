#ifndef DEVGUI_H
#define DEVGUI_H

namespace dev {
extern bool g_hud_enabled;
void Init();
void Update();
void Render();
void Quit();
}  // namespace dev

#endif  // DEVGUI_H
