#include <fstream>
#include <stdint.h>
#include <time.h>
#include <chrono>

#include "Platform.hpp"
#include "Chip8VM.hpp"

int main(int argc, char* argv[]) {
  bool kill = false;
  uint8_t keypad[16]{};
  Chip8VM vm;
  Platform platform(argv[1]);
  int cycleDelay;
  if (argc > 1) {
    cycleDelay = std::stoi(argv[1]);
    vm.load_rom(argv[2]);
  } else {
    std::cout << "Please provide a ROM file\n";
    exit(0);
  }

  while (!kill) {
    kill = platform.read_input(keypad);

    Uint64 start = SDL_GetPerformanceCounter();
    for (uint16_t i = 0; i < cycleDelay/FPS; i++)
      vm.execute_cycle(keypad);
    Uint64 end = SDL_GetPerformanceCounter();

    auto elapsedMS = ((end - start) * 1000) / SDL_GetPerformanceFrequency();

    SDL_Delay((1000.0/FPS) > elapsedMS ? ((1000.0/FPS) - elapsedMS) : 0);
    platform.render(vm.get_video_memory());
    vm.update_timers();
  }
  return 0;
}
