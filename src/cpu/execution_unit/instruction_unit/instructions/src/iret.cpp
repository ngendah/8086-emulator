#include "iret.h"

#include "constants/opcodes.h"
#include "instruction_templates.h"
#include "pop.h"

IRET::IRET(bus_ptr_t bus, registers_ptr_t registers)
    : MicroOp(bus, registers) {}

void IRET::execute(UNUSED_PARAM const Instruction &) {
  PopFlags pop_flags{_bus, _registers};
  PopSegment pop_segment{_bus, _registers};
  PopIP pop_register{_bus, _registers};
  pop_register.execute(Instruction());
  {
    sr_t mod{0, SegmentMapper::CS_INDEX, 0};
    pop_segment.execute(Instruction(
        SOP::NONE, make_word(cast_ui8(OPCODES::POP_CS), cast_ui8(mod))));
  }
  pop_flags.execute(Instruction(SOP::NONE, cast_ui8(OPCODES::POPF)));
}
