#ifndef GLOBAL_H
#define GLOBAL_H

class GLFWwindow;
class Level;
class Camera;

namespace Engine {
  extern GLFWwindow* window;
  extern int width;
  extern int height;
  extern Level* level;
  extern Camera camera;
}

#endif // GLOBAL_H
