#include "instruction_set.h"
#include "mov.h"
#include "pop.h"
#include "push.h"
#include "xchg.h"
#include <algorithm>

#define _INSTRUCTION(opcode, mask, memonic, cls)                               \
  register_instruction<cls>(MicroOp::Key(opcode, mask, memonic))

InstructionSet::InstructionSet() {
  _INSTRUCTION(0x88, 0xFC, "MOV", MovRegisterAndMemory);
  _INSTRUCTION(0xB0, 0xF0, "MOV", MovRegisterImmediate);
  _INSTRUCTION(0xA0, 0xF0, "MOV", MovAccumulator);
  _INSTRUCTION(0x8E, 0xFC, "MOV", MovSegmentAndRegisterMemory);
  _INSTRUCTION(0xC6, 0xFE, "MOV", MovMemoryImmediate);
  _INSTRUCTION(0x50, 0xF0, "PUSH", PushRegister);
  _INSTRUCTION(0xFF, 0xFF, "PUSH", PushMemory);
  _INSTRUCTION(0x06, 0xE7, "PUSH", PushSegment);
  // _INSTRUCTION(0x9C, 0xFF, "PUSH", PushFlags);
  _INSTRUCTION(0x58, 0xF8, "POP", PopRegister);
  _INSTRUCTION(0x8F, 0xFF, "POP", PopMemory);
  _INSTRUCTION(0x07, 0xE7, "POP", PopSegment);
  // _INSTRUCTION(0x9D, 0xFF, "POP", PopFlags);
  _INSTRUCTION(0x90, 0xF8, "XCHG", XCHGAccumulator);
  _INSTRUCTION(0x86, 0xFE, "XCHG", XCHGRegisterMemory);
}

InstructionSet::create_func_t InstructionSet::find(const MicroOp::Key &key) {
  auto it =
      std::find_if(instructions.begin(), instructions.end(),
                   [&key](const std::pair<MicroOp::Key, create_func_t> &rhs) {
                     return rhs.first == key;
                   });
  if (it == std::end(instructions)) {
    assert(false);
  }
  return it->second;
}

std::shared_ptr<MicroOp> InstructionSet::decode(uint8_t opcode,
                                                const MicroOp::Params &params) {
  return find(MicroOp::Key(opcode))(params);
}