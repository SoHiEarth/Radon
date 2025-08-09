#ifndef DEVGUI_H
#define DEVGUI_H

namespace dev {
  extern bool hud_enabled;
  void Init();
  void Update();
  void LateUpdate();
  void Quit();
}

#endif // DEVGUI_H
