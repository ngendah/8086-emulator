#include "decoder_impl.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

#include "instruction_set/mov.h"

#define INVALID InstructionCode(0x0, 0x0)
#define NOP InstructionCode(0x90, 0xff)

DecoderImpl::DecoderImpl(BUS *bus, Clock *clock, Registers *registers)
    : _registers(registers), _bus(bus), _clock(clock) {
  _instructions = {
      INVALID,
      NOP,
  };
  _clock = nullptr;
}

DecoderImpl::~DecoderImpl() {}

void DecoderImpl::decodeAndExecute() {
  sop_t sop = fetch(sizeof(sop_t));
  if (sop == 0x26 || sop == 0x2E || sop == 0x36 || sop == 0x3E) {
    // uint8_t byte = fetch(sizeof(uint8_t));
  } else {
    // sop = 0x0;
  }
}

uint16_t DecoderImpl::fetch(uint16_t len) {
  Address addr = _registers->CS.address(_registers->IP);
  uint16_t data = _bus->read(&addr, len);
  _registers->IP += len;
  return data;
}

InstructionCode DecoderImpl::executioner(uint8_t opcode) {
  auto opcodeToInstruction =
      std::find(_instructions.begin(), _instructions.end(), opcode);
  if (opcodeToInstruction != std::end(_instructions)) {
    return *opcodeToInstruction;
  } else {
    return _instructions[0];
  }
}
