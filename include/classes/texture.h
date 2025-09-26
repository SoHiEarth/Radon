#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

struct Texture {
  unsigned int id_ = 0;
  int width_ = 0, height_ = 0, channels_ = 0;
  const std::string path_;
  Texture(std::string_view path) : path_(path) {}
};

#endif  // TEXTURE_H