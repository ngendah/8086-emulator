#include "int.h"

#include "constants/opcodes.h"
#include "instruction_templates.h"
#include "push.h"

void INT::before_execute(UNUSED_PARAM const Instruction &) {
  PushFlags push_flags{_bus, _registers};
  PushSegment push_segment{_bus, _registers};
  PushIP push_ip{_bus, _registers};
  push_flags.execute(Instruction(SOP::NONE, cast_ui8(OPCODES::PUSHF)));
  {
    sr_t mod{0, SegmentMapper::CS_INDEX, 0};
    push_segment.execute(Instruction(
        SOP::NONE, make_word(cast_ui8(OPCODES::PUSH_CS), cast_ui8(mod))));
  }
  push_ip.execute(Instruction());
}

void INT::execute(const Instruction &instruction) {
  const uint16_t offset = instruction.data<uint8_t>() * 4; // TODO use <<
  auto address = Address(offset);
  auto *memory = _bus->memory();
  _registers->IP = memory->read(&address, sizeof_ui16);
  address += sizeof_ui16;
  _registers->CS = memory->read(&address, sizeof_ui16);
}
