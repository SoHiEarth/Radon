#ifndef TEXTURE_H
#define TEXTURE_H

struct Texture {
  unsigned int id_;
  int width_, height_, channels_;
  std::string type_;
  std::string path_;
};

#endif  // TEXTURE_H
