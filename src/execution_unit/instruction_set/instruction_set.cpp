#include "instruction_set.h"

#include "mov.h"
#include "pop.h"
#include "push.h"
#include "xchg.h"

InstructionSet::InstructionSet() {
  register_instruction<MovRegisterAndMemory>(MicroOp::Key(0x88, 0xFC));
  register_instruction<MovRegisterImmediate>(MicroOp::Key(0xC6, 0xF6));
  register_instruction<MovAccumulator>(MicroOp::Key(0xA0, 0xF0));
  register_instruction<MovSegmentAndRegisterMemory>(MicroOp::Key(0x8E, 0xFC));
}

std::shared_ptr<MicroOp> InstructionSet::decode(uint8_t opcode,
                                                const MicroOp::Params &params) {
  auto it = instructions.find(MicroOp::Key(opcode));
  if (it == instructions.end()) {
    assert(false);
  }
  return it->second(params);
}