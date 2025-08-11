#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

struct Texture {
  unsigned int id;
  int w, h, channels;
  const std::string path;
  void Use() const;
  Texture(const std::string_view path) : path(path) {};
};

#endif // TEXTURE_H
