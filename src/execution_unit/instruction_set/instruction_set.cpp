#include "instruction_set.h"
#include "in.h"
#include "lahf.h"
#include "lds.h"
#include "lea.h"
#include "les.h"
#include "mov.h"
#include "out.h"
#include "pop.h"
#include "push.h"
#include "sahf.h"
#include "xchg.h"
#include "xlat.h"

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
  _INSTRUCTION(0xE4, 0xFE, "IN", INPort);
  _INSTRUCTION(0xEC, 0xFE, "IN", INDX);
  _INSTRUCTION(0xE6, 0xFE, "OUT", OUTPort);
  _INSTRUCTION(0xEE, 0xFE, "OUT", OUTDX);
  _INSTRUCTION(0xD7, 0xD6, "XLAT", XLAT);
  _INSTRUCTION(0x8D, 0xFF, "LEA", LEA);
  _INSTRUCTION(0xC4, 0xFF, "LES", LES);
  _INSTRUCTION(0xC5, 0xFF, "LDS", LDS);
  _INSTRUCTION(0x9F, 0xFF, "LAHF", LAHF);
  _INSTRUCTION(0x9E, 0xFF, "SAHF", SAHF);
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