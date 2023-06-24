#include "int.h"

#include "constants/opcodes.h"
#include "instruction_templates.h"
#include "push.h"

void INT::before_execute(UNUSED_PARAM const Instruction &) {
  PushFlags push_flags{_bus, _registers};
  PushSegment push_segment{_bus, _registers};
  PushIP push_ip{_bus, _registers};
  PLOGD << "Push flags";
  push_flags.execute(Instruction(SOP::NONE, (uint8_t)OPCODES::PUSHF));
  {
    PLOGD << "Push CS";
    sr_t mod{0, SegmentMapper::CS_INDEX, 0};
    push_segment.execute(Instruction(
        SOP::NONE, make_word((uint8_t)OPCODES::PUSH_CS, (uint8_t)mod)));
  }
  PLOGD << "Push IP";
  push_ip.execute(Instruction());
}

void INT::execute(const Instruction &instruction) {
  uint16_t offset = instruction.data<uint8_t>() * 4; // TODO use <<
  auto address = Address(offset);
  _registers->IP = _bus->read(&address, sizeof(uint16_t));
  address += sizeof(uint16_t);
  _registers->CS = _bus->read(&address, sizeof(uint16_t));
}
