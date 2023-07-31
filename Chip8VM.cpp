#include "Chip8VM.hpp"

// Private
void Chip8VM::_00ex(uint16_t rest) {
  if ((rest & 255) == 0xE0) {
    // Instruction: 00E0
    memset(video, 0, sizeof(video));
    log(VM, INFO, "00E0 at [PC %#04x]\t Clearing screen", pc - 2);
  } else if ((rest & 255) == 0xEE){
    // Instruction: 00EE
    if (sp != -1) {
      uint16_t prev_pc = pc;
      pc = stack[sp--];
      log(VM, INFO, "00EE at [PC %#04x]\t Popping stack, PC now %#04x", prev_pc - 2, pc);
    } else {
      log(VM, CRITICAL, "00EE at [PC %#04x]\t Stack Underflow", pc - 2);
    }
  }
}

void Chip8VM::_1nnn(uint16_t rest) {
  log(VM, INFO, "1%03x at [PC %#04x]\t PC set to %#04x", rest, pc-2, rest);
  pc = rest;
}


void Chip8VM::_6xnn(uint16_t rest) {
  uint8_t vx = (rest & 0xF00) >> 8u;
  uint8_t nn = rest & 0x0FF;
  log(VM, INFO, "6%03x at [PC %#04x]\t Register %d(%d) set to value %d", rest, pc-2, vx, registers[vx], nn);
  registers[vx] = nn;
}


void Chip8VM::_7xnn(uint16_t rest) {
  uint8_t vx = (rest & 0xF00) >> 8u;
  uint8_t nn = rest & 0x0FF;
  auto temp = registers[vx];
  registers[vx] = registers[vx] + nn;

  log(VM, INFO, "7%03x at [PC %#04x]\t Value of %d added to R%d(%d): %d", rest, pc-2, nn, vx, temp, registers[vx]);
}


void Chip8VM::_annn(uint16_t rest) {
  uint16_t nnn = rest & 0xFFF;
  log(VM, INFO, "A%03x at [PC %#04x]\t Index register(%d) is set to %d", rest, pc-2, index, nnn);
  index = nnn;
}


void Chip8VM::_dxyn(uint16_t rest) {
  int vx = (rest & 0XF00) >> 8u;
  int vy = (rest & 0X0F0) >> 4u;
  int rx = registers[vx] % 64;
  int ry = registers[vy] % 32;

  int n = rest & 0x00F;
  registers[0xF] = 0;
  for (int i = 0; i < n; i++) {
    if ((ry+i) == 32) break;
    int data = memory[index+i];
    for (int j = 0; j < 8; j++) {
      if ((rx+j) == 64) break;
      uint8_t pixel_sprite = data & (0x80 >> j);
      uint32_t *pixel_video = &video[(ry + i) * 64 + (rx + j)];
      if (pixel_sprite) {// Screen pixel also on - collision
        if (*pixel_video == 0xFFFFFFFF) {
          registers[0xF] = 1;
        }
        *pixel_video ^= 0xFFFFFFFF;
      }
    }
  }

  log(VM, INFO, "D%03x at [PC %#04x]\t Drawing at %d(R%d), %d(R%d)", rest, pc-2, registers[vx], vx, registers[vy], vy);
}


void Chip8VM::_3xnn(uint16_t rest) {
  int vx = (rest & 0xF00) >> 8u;
  int nn = (rest & 0x0FF);

  if (registers[vx] == nn) {
    log(VM, INFO, "3%03x at [PC %#04x]\t R%d(%d) == %d: skipping instruction, PC now %#04x", rest, pc-2, vx, registers[vx], nn, pc+2);
    pc += 2;
  } else {
    log(VM, INFO, "3%03x at [PC %#04x]\t R%d(%d) != %d: NOT skipping instruction, PC now %#04x", rest, pc-2, vx, registers[vx], nn, pc);
  }
}


void Chip8VM::_4xnn(uint16_t rest) {
  int vx = (rest & 0xF00) >> 8u;
  int nn = (rest & 0x0FF);


  if (registers[vx] != nn) {
    log(VM, INFO, "4%03x at [PC %#04x]\t R%d(%d) != %d: skipping instruction, PC now %#04x", rest, pc-2, vx, registers[vx], nn, pc+2);
    pc += 2;
  } else {
    log(VM, INFO, "4%03x at [PC %#04x]\t R%d(%d) == %d: NOT skipping instruction, PC now %#04x", rest, pc-2, vx, registers[vx], nn, pc);
  }
}


void Chip8VM::_5xy0(uint16_t rest) {
  int vx = (rest & 0xF00) >> 8u;
  int vy = (rest & 0x0F0) >> 4u;


  if (registers[vx] == registers[vy]) {
    log(VM, INFO, "5%03x at [PC %#04x]\t R%d(%d) == R%d(%d): skipping instruction, PC now %#04x", rest, pc-2, vx, registers[vx], vy, registers[vy], pc+2);
    pc += 2;
  } else {
    log(VM, INFO, "5%03x at [PC %#04x]\t R%d(%d) != R%d(%d): NOT skipping instruction, PC now %#04x", rest, pc-2, vx, registers[vx], vy, registers[vy], pc);
  }
}


void Chip8VM::_9xy0(uint16_t rest) {
  int vx = (rest & 0xF00) >> 8u;
  int vy = (rest & 0x0F0) >> 4u;


  if (registers[vx] != registers[vy]) {
    log(VM, INFO, "9%03x at [PC %#04x]\t R%d(%d) != R%d(%d): skipping instruction, PC now %#04x", rest, pc-2, vx, registers[vx], vy, registers[vy], pc+2);
    pc += 2;
  } else {
    log(VM, INFO, "9%03x at [PC %#04x]\t R%d(%d) == R%d(%d): NOT skipping instruction, PC now %#04x", rest, pc-2, vx, registers[vx], vy, registers[vy], pc);
  }
}


void Chip8VM::_2nnn(uint16_t rest) {
  if (sp < 16) {
    auto prev_pc = pc;
    stack[++sp] = pc;
    pc = rest;
    log(VM, INFO, "2%03x at [PC %#04x]\t Pushing current PC to stack, PC now %#04x", rest, prev_pc-2, pc);
  }
}


void Chip8VM::_8xyn(uint16_t rest) {
  int vx = (rest & 0XF00) >> 8u;
  int vy = (rest & 0X0F0) >> 4u;
  int n = rest & 0x00F;


  uint8_t rx;
  uint8_t ry;

  switch(n) {
  case 0:
    // Set
    registers[vx] = registers[vy];
    log(VM, INFO, "8%03x at [PC %#04x]\t R%d set to value of R%d", rest, pc-2, vx, vy);
    break;
  case 1:
    // Binary OR
    registers[vx] = registers[vx] | registers[vy];
    log(VM, INFO, "8%03x at [PC %#04x]\t R%d set to OR of R%d and R%d", rest, pc-2, vx, vx, vy);
    break;
  case 2:
    // Binary AND
    registers[vx] = registers[vx] & registers[vy];
    log(VM, INFO, "8%03x at [PC %#04x]\t R%d set to AND of R%d and R%d", rest, pc-2, vx, vx, vy);
    break;
  case 3:
    // Logical XOR
    registers[vx] = registers[vx] ^ registers[vy];
    log(VM, INFO, "8%03x at [PC %#04x]\t R%d set to XOR of R%d and R%d", rest, pc-2, vx, vx, vy);
    break;
  case 4:
    // Add
    rx = registers[vx];
    ry = registers[vy];
    if (ry > UINT8_MAX - rx) {
      registers[0xF] = 1;
    } else {
      registers[0xF] = 0;
    }
    registers[vx] = (rx + ry) & 0xFFu;
    log(VM, INFO, "8%03x at [PC %#04x]\t R%d set to sum of R%d(%d) and R%d(%d): %d", rest, pc-2, vx, vx, rx, vy, ry, registers[vx]);
    break;
  case 5:
    // Subtract vx - vy
    rx = registers[vx];
    ry = registers[vy];
    if (rx > ry) {
      registers[0xF] = 1;
    } else {
      registers[0xF] = 0;
    }
    registers[vx] = rx - ry;
    log(VM, INFO, "8%03x at [PC %#04x]\t R%d set to difference of R%d and R%d", rest, pc-2, vx, vx, vy);
    break;
  case 7:
    // Subtract vy - vx
    rx = registers[vx];
    ry = registers[vy];
    if (ry > rx) {
      registers[0xF] = 1;
    } else {
      registers[0xF] = 0;
    }
    registers[vx] = ry - rx;
    log(VM, INFO, "8%03x at [PC %#04x]\t R%d set to difference of R%d and R%d", rest, pc-2, vx, vy, vx);
    break;
  case 6:
    // Shift right
    rx = registers[vx];
    registers[0xF] = (registers[vx] & 0x1u);
    registers[vx] >>= 1;

    log(VM, INFO, "8%03x at [PC %#04x]\t R%d shifted right. %04x to %04x", rest, pc-2, vx, rx, registers[vx]);
    break;
  case 0xE:
    // Shift left
    rx = registers[vx];
    registers[0xF] = (registers[vx] & 0x80u) >> 7u;
    registers[vx] <<= 1;
    log(VM, INFO, "8%03x at [PC %#04x]\t R%d shifted left. %04x to %04x", rest, pc-2, vx, rx, registers[vx]);
    //ry = registers[vy];
    break;
  default:
    log(VM, CRITICAL, "Invalid instruction 8%03x", rest);
    break;
  }
}


void Chip8VM::_bnnn(uint16_t rest) {
  uint8_t offset = registers[0];
  log(VM, INFO, "B%03x at [PC %#04x]\t Changing PC to %#04x", rest, pc - 2, offset + rest);
  pc = offset + rest;
}


void Chip8VM::_cxnn(uint16_t rest) {
  // TODO: Find a better random number generator?
  int vx = (rest & 0xF00) >> 8u;
  int nn = rest & 0x0FF;

  registers[vx] = (rand() % 255) & nn;
  log(VM, INFO, "C%03x at [PC %#04x]\t Generated random number %d", rest, pc - 2, registers[vx]);
}


void Chip8VM::_ex(uint16_t rest) {
  int vx = (rest & 0xF00) >> 8u;
  int code = rest & 0x0FF;
  int key;

  switch(code) {
  case 0x9E:
    key = registers[vx];
    if (keypad[key] == 1) {
      log(VM, INFO, "E%03x at [PC %#04x]\t Key(%d) pressed, skipping an instruction", rest, pc - 2, vx);
      pc += 2;
    } else {
      log(VM, INFO, "E%03x at [PC %#04x]\t Key(%d) NOT pressed, NOT skipping an instruction", rest, pc - 2, vx);
    }
    break;
  case 0xA1:
    key = registers[vx];
    if (keypad[key] != 1) {
      log(VM, INFO, "E%03x at [PC %#04x]\t Key(%d) not pressed, skipping an instruction", rest, pc - 2, vx);
      pc += 2;
    } else {
      log(VM, INFO, "E%03x at [PC %#04x]\t Key(%d) pressed, NOT skipping an instruction", rest, pc - 2, vx);
    }
    break;
  default:
    log(VM, CRITICAL, "Invalid instruction E%03x", rest);
    break;
  }
}

void Chip8VM::_fxnn(uint16_t rest) {
  int vx = (rest & 0xF00) >> 8u;
  int nn = rest & 0x0FF;
  int flag = 0;
  int rx;
  uint16_t temp_index;

  switch(nn) {
  case 0x07:
    registers[vx] = delay;
    log(VM, INFO, "F%03x at [PC %#04x]\t R%d set to delay timer value of %04x", rest, pc - 2, vx, delay);
    break;
  case 0x15:
    delay = registers[vx];
    log(VM, INFO, "F%03x at [PC %#04x]\t Delay timer set to value of R%d: %04x", rest, pc - 2, vx, registers[vx]);
    break;
  case 0x18:
    sound = registers[vx];
    log(VM, INFO, "F%03x at [PC %#04x]\t Sound timer set to value of R%d: %04x", rest, pc - 2, vx, registers[vx]);
    break;
  case 0x1E:
    temp_index = index;
    index += registers[vx];
    log(VM, INFO, "F%03x at [PC %#04x]\t Index register(%d) added by value of R%d(%d): %d", rest, pc - 2, temp_index, vx, registers[vx], index);
    break;
  case 0x0A:
    // TODO: Is this stupid?
    flag = 0;
    for (int i=0; i < 16; i++) {
      if (keypad[i] == 1) {
        registers[vx] = i;
        flag = 1;
        log(VM, INFO, "F%03x at [PC %#04x]\t Key %x is active, continuing", rest, pc - 2, vx);
        break;
      }
    }
    if (flag == 0) {
      log(VM, INFO, "F%03x at [PC %#04x]\t No key pressed, looping", rest, pc - 2);
      pc -= 2;
    }
    break;
  case 0x29:
    index = FONTSET_START_ADDRESS + (registers[vx] * 5u);
    log(VM, INFO, "F%03x at [PC %#04x]\t Setting index register to font value %x at %04x", rest, pc - 2, vx, FONTSET_START_ADDRESS + (registers[vx] * 5u));
    break;
  case 0x33:
    rx = registers[vx];

    memory[index+2] = rx % 10;
    rx /= 10;

    memory[index+1] = rx % 10;
    rx /= 10;

    memory[index] = rx;

    log(VM, INFO, "F%03x at [PC %#04x]\t BCD number %d stored as %d, %d and %d", rest, pc - 2, registers[vx], memory[index], memory[index+1], memory[index+2]);

    break;
  case 0x55:
    temp_index = index;
    for (int i = 0; i <= vx; i++) {
      memory[temp_index] = registers[i];
      temp_index++;
    }
    log(VM, INFO, "F%03x at [PC %#04x]\t Updated memory locations from %04x to %04x", rest, pc - 2, index, temp_index);
    break;
  case 0x65:
    temp_index = index;
    for (int i = 0; i <= vx; i++) {
      registers[i] = memory[temp_index];
      temp_index++;
    }
    log(VM, INFO, "F%03x at [PC %#04x]\t Updated registers from memory locations %04x to %04x", rest, pc - 2, index, temp_index);
    break;
  default:
    log(VM, CRITICAL, "Invalid instruction F%03x", rest);
    break;
  }
}


void Chip8VM::decode_execute(uint16_t instruction) {
  int opcode = (instruction & 61440) >> 12u;
  int rest = instruction & 4095;

  switch (opcode) {
  case 0:
    _00ex(rest);
    break;
  case 0x2:
    _2nnn(rest);
    break;
  case 0x1:
    _1nnn(rest);
    break;
  case 0x3:
    _3xnn(rest);
    break;
  case 0x4:
    _4xnn(rest);
    break;
  case 0x5:
    _5xy0(rest);
    break;
  case 0x6:
    _6xnn(rest);
    break;
  case 0x7:
    _7xnn(rest);
    break;
  case 0x8:
    _8xyn(rest);
    break;
  case 0x9:
    _9xy0(rest);
    break;
  case 0xA:
    _annn(rest);
    break;
  case 0xB:
    _bnnn(rest);
    break;
  case 0xC:
    _cxnn(rest);
    break;
  case 0xD:
    _dxyn(rest);
    break;
  case 0xE:
    _ex(rest);
    break;
  case 0xF:
    _fxnn(rest);
    break;
  default:
    break;
  }
}

// Public
Chip8VM::Chip8VM() {
  pc = start;
  // Load fonts into memory
  for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
    memory[FONTSET_START_ADDRESS + i] = fontset[i];
  }

  srand(time(0));
}


void Chip8VM::load_rom(char const *filename) {
  std::ifstream fin(filename, std::ios::binary | std::ios::ate);

  if (fin.is_open()) {
    std::streampos size = fin.tellg();
    fin.seekg (0, std::ios::beg);

    char *buffer = new char[size];
    fin.read(buffer,size);
    fin.close();

    for (int i = 0; i < size; i++) {
      memory[start + i] = buffer[i];
    }

    delete[] buffer;
  } else {
    std::cout << "Error reading ROM file\n";
    exit(0);
  }
}


void Chip8VM::execute_cycle(uint8_t *keypad) {
  uint16_t instruction = (memory[pc] << 8) | (memory[pc+1]);
  pc += 2;
  memcpy(this->keypad, keypad, sizeof(this->keypad));
  decode_execute(instruction);
}


uint32_t* Chip8VM::get_video_memory() {
  return video;
}

void Chip8VM::update_timers() {
  delay = (delay > 0) ? delay - 1 : 0;
  sound = (sound > 0) ? delay - 1 : 0;
}
