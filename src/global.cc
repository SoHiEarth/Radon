#include <engine/global.h>
#include <classes/camera.h>

GLFWwindow* Engine::window = nullptr;
int         Engine::width = 800;
int         Engine::height = 600;
Level*      Engine::level = nullptr;
Camera      Engine::camera;
