#ifndef MISC_H_
#define MISC_H_

#include <iostream>
#include <string.h>
#include <SDL2/SDL.h>

#define INFO SDL_LOG_PRIORITY_INFO
#define CRITICAL SDL_LOG_PRIORITY_CRITICAL

#define VM 0
#define PLATFORM 1
#define DEBUG 1

#define FPS 60

void log(int location, SDL_LogPriority priority, const char *fmt, ...);


#endif // MISC_H_
