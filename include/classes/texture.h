#ifndef CLASSES_TEXTURE_H
#define CLASSES_TEXTURE_H

#include <string>

struct Texture {
  unsigned int id;
  int w, h, channels;
  const std::string path;
  void Use() const;
  Texture(const std::string_view path) : path(path) {};
};

#endif // CLASSES_TEXTURE_H
