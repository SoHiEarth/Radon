#pragma once
#include <cstring>
struct Texture {
  unsigned int id_ = 0;
  int width_ = 0, height_ = 0, channels_ = 0;
  const char* name_ = "";
  const char* path_ = "";
  explicit Texture(const char* path) {
    path_ = strcpy(new char[strlen(path) + 1], path);
  }
};
