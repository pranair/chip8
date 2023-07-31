#ifndef CHIP8VM_H_
#define CHIP8VM_H_

#include <fstream>
#include "Misc.hpp"

class Chip8VM {
private:
  // Font
  const unsigned int FONTSET_SIZE = 80;
  const unsigned int FONTSET_START_ADDRESS = 0x50;
  uint8_t fontset[80] =
    {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

  // General
  uint8_t memory[4096]{};
  uint32_t video[64*32]{};
  uint16_t stack[16]{};
  uint8_t registers[16]{};
  uint8_t keypad[16]{};
  uint16_t index{};
  uint16_t pc{};
  uint8_t delay{};
  uint8_t sound{};
  int sp = -1;
  const unsigned int start = 0x200;

  // Instructions
  void _00ex(uint16_t);
  void _1nnn(uint16_t);
  void _6xnn(uint16_t);
  void _7xnn(uint16_t);
  void _annn(uint16_t);
  void _dxyn(uint16_t);
  void _3xnn(uint16_t);
  void _4xnn(uint16_t);
  void _5xy0(uint16_t);
  void _9xy0(uint16_t);
  void _2nnn(uint16_t);
  void _8xyn(uint16_t);
  void _bnnn(uint16_t);
  void _cxnn(uint16_t);
  void _ex(uint16_t);
  void _fxnn(uint16_t);
  void decode_execute(uint16_t);

public:
  Chip8VM();
  void load_rom(char const *filename);
  void execute_cycle(uint8_t *keypad);
  uint32_t *get_video_memory();
  void update_timers();
};


#endif // CHIP8VM_H_
