#include <classes/camera.h>
#include <engine/global.h>
enum { kDefaultWindowWidth = 800, kDefualtWindowHeight = 600 };

GLFWwindow* Engine::g_window = nullptr;
int Engine::g_width = kDefaultWindowWidth;
int Engine::g_height = kDefualtWindowHeight;
Level* Engine::g_level = nullptr;
Camera Engine::g_camera;
