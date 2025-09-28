#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

struct Texture {
  unsigned int id_ = 0;
  int width_ = 0, height_ = 0, channels_ = 0;
  std::string name;
  const std::string kPath;
  Texture(std::string_view path) : kPath(path) {}
};

#endif  // TEXTURE_H