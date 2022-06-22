#ifndef INSTRUCTION_SET_PHYSICAL_ADDRESSER_H_
#define INSTRUCTION_SET_PHYSICAL_ADDRESSER_H_

#include <array>
#include <functional>

#include "logger.h"
#include "types.h"

class PhysicalAddresser {
  typedef Address (PhysicalAddresser::*fn_t)(Segment *, const uint16_t &);

protected:
  Registers *_registers;
  std::array<fn_t, 8> _mapper;

  Address _f_1(Segment *segment, const uint16_t &eff_addr) {
    return segment->address(eff_addr);
  }

  Address _f0(Segment *segment, const uint16_t &offset) {
    return segment->address(_registers->BX.read() + _registers->SI.read() +
                            offset);
  }

  Address _f1(Segment *segment, const uint16_t &offset) {
    return segment->address(_registers->BX.read() + _registers->DI.read() +
                            offset);
  }

  Address _f2(Segment *segment, const uint16_t &offset) {
    return segment->address(_registers->BP.read() + _registers->SI.read() +
                            offset);
  }

  Address _f3(Segment *segment, const uint16_t &offset) {
    return segment->address(_registers->BP.read() + _registers->DI.read() +
                            offset);
  }

  Address _f4(Segment *segment, const uint16_t &offset) {
    return segment->address(_registers->SI, offset);
  }

  Address _f5(Segment *segment, const uint16_t &offset) {
    return segment->address(_registers->DI, offset);
  }

  Address _f6(Segment *segment, const uint16_t &offset) {
    return segment->address(_registers->BP, offset);
  }

  Address _f7(Segment *segment, const uint16_t &offset) {
    return segment->address(_registers->BX, offset);
  }

public:
  PhysicalAddresser(Registers *registers) : _registers(registers) {
    _mapper = {
        &PhysicalAddresser::_f0, &PhysicalAddresser::_f1,
        &PhysicalAddresser::_f2, &PhysicalAddresser::_f3,
        &PhysicalAddresser::_f4, &PhysicalAddresser::_f5,
        &PhysicalAddresser::_f6, &PhysicalAddresser::_f7,
    };
  }

  Address address(Segment *segment, uint8_t idx, uint16_t offset) {
    PLOGD << "physical address mapping: " << *segment
          << fmt::format(", addressing_mode=0x{0:x}, offset=0x{1:x}", idx,
                         offset);
    fn_t _map_fn = _mapper[idx];
    return (this->*_map_fn)(segment, offset);
  }

  Address address(Segment *segment, uint8_t idx, uint8_t offset) {
    PLOGD << "physical address mapping: " << *segment
          << fmt::format(", addressing_mode=0x{0:x}, offset=0x{1:x}", idx,
                         offset);
    fn_t _map_fn = _mapper[idx];
    return (this->*_map_fn)(segment, offset);
  }

  Address address(Segment *segment, uint8_t idx) {
    PLOGD << "physical addresser mapping " << *segment
          << fmt::format(", addressing_mode=0x{:x}", idx);
    fn_t _map_fn = _mapper[idx];
    return (this->*_map_fn)(segment, 0x0);
  }

  Address address(Segment *segment, uint16_t offset = 0) {
    PLOGD << "direct physical addresser " << *segment
          << fmt::format(", offset=0x{1:x}", offset);
    fn_t _map_fn = &PhysicalAddresser::_f_1;
    return (this->*_map_fn)(segment, offset);
  }
};

#endif // INSTRUCTION_SET_PHYSICAL_ADDRESSER_H_
