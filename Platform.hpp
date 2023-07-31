#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "Misc.hpp"
#include <cstdint>

#define SCALE 20

class Platform {
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

public:
  Platform(const char *);
  bool read_input(uint8_t *);
  void render(uint32_t *);
};

#endif // PLATFORM_H_
