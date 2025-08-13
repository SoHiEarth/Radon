#ifndef GLOBAL_H
#define GLOBAL_H

class GLFWwindow;
class Level;
class Camera;

namespace Engine {
extern GLFWwindow* g_window;
extern int g_width;
extern int g_height;
extern Level* g_level;
extern Camera g_camera;
}  // namespace Engine

#endif  // GLOBAL_H
