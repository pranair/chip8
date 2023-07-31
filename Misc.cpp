#include "Misc.hpp"

#ifdef DEBUG
void log(int location, SDL_LogPriority priority, const char *fmt, ...) {
  // NOTE: First instinct was to use vsnprintf
  //       Turns out just like printf and vprintf,
  //       differet variants of print function exists to accept
  //       vlist (variadic) arguments
  static std::string last_line;
  static unsigned int last_line_count;
  int size = sizeof(fmt) * 20;
  char *buffer = new char[size];
  char *current_line = new char[size];
  char prefix[10] = "";
  va_list ap;

  if (location == VM) {
    strcpy(prefix, "VM");
  } else if (location == PLATFORM) {
    strcpy(prefix, "PLATFORM");
  }

  snprintf(buffer, size, "[%s] %s", prefix, fmt);

  va_start(ap, fmt);

  vsnprintf(current_line, size, buffer, ap);
  if (std::string(current_line) != last_line) {
    SDL_LogMessage(0, priority, current_line);
    last_line = std::string(current_line);
    last_line_count = 0;
  } else {
    last_line_count++;
    SDL_LogMessage(0, priority, "%s [x%d]\033[1A\033[K", current_line, last_line_count);
  }

  va_end(ap);

  delete[] buffer;
  delete[] current_line;
}
#else
void log(int location, SDL_LogPriority priority, const char *fmt, ...) {}
#endif
