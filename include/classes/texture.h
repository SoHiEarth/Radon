#pragma once
struct Texture {
  unsigned int id_ = 0;
  int width_ = 0, height_ = 0, channels_ = 0;
  const char* name = "";
  const char* kPath = "";
  Texture(const char* path) {
    kPath = strcpy(new char[strlen(path) + 1], path);
  }
};