#include <engine/global.h>
#include <atomic>
#include <GLFW/glfw3.h>

_WINDOW_ATOMIC Engine::window{nullptr};
_INT_ATOMIC    Engine::width{800};
_INT_ATOMIC    Engine::height{600};
_LEVEL_ATOMIC  Engine::current_level{nullptr};
