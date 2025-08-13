#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

struct Texture {
  unsigned int id_;
  int w_, h_, channels_;
  const std::string kPath;
  void Use() const;
  explicit Texture(const std::string_view kPath) : kPath(kPath) {};
};

#endif  // TEXTURE_H
