#include "instruction_set.h"

#include "mov.h"
#include "pop.h"
#include "push.h"
#include "xchg.h"

#define _INSTRUCTION(opcode, mask, cls)                                        \
  register_instruction<cls>(MicroOp::Key(opcode, mask))

InstructionSet::InstructionSet() {
  _INSTRUCTION(0x88, 0xFC, MovRegisterAndMemory);
  _INSTRUCTION(0xB0, 0xF0, MovRegisterImmediate);
  _INSTRUCTION(0xA0, 0xF0, MovAccumulator);
  _INSTRUCTION(0x8E, 0xFC, MovSegmentAndRegisterMemory);
  _INSTRUCTION(0xC6, 0xFE, MovMemoryImmediate);
  _INSTRUCTION(0x50, 0xF0, PushRegister);
  _INSTRUCTION(0xFF, 0xFF, PushMemory);
  _INSTRUCTION(0x06, 0xE7, PushSegment);
}

InstructionSet::create_func_t InstructionSet::find(const MicroOp::Key &key) {
  auto it = instructions.find(key);
  if (it == instructions.end()) {
    assert(false);
  }
  return it->second;
}

std::shared_ptr<MicroOp> InstructionSet::decode(uint8_t opcode,
                                                const MicroOp::Params &params) {
  return find(MicroOp::Key(opcode))(params);
}