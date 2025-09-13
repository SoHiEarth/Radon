#ifndef TEXTURE_H
#define TEXTURE_H

struct Texture {
  unsigned int id_ = 0;
  int width_ = 0, height_ = 0, channels_ = 0;
  std::string path_ = "";
};

#endif  // TEXTURE_H