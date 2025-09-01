#ifndef TEXTURE_H
#define TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

struct Texture {
  unsigned int id_;
//  int w_, h_, channels_;
  const char* type_;
  const char* path_;
};

#ifdef __cplusplus
}
#endif

#endif  // TEXTURE_H
