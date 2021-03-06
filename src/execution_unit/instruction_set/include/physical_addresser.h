//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef INSTRUCTION_SET_PHYSICAL_ADDRESSER_H_
#define INSTRUCTION_SET_PHYSICAL_ADDRESSER_H_

#include <array>
#include <functional>

#include "logger.h"
#include "types.h"

class EffectiveAddresser {
  typedef Address (EffectiveAddresser::*fn_t)(const uint16_t &);

protected:
  Registers *_registers;
  std::array<fn_t, 8> _eff_mapper;
  std::array<std::string, 8> _eff_memonics;

  Address _address(const Register &r1, const Register &r2,
                   const uint16_t &offset) {
    PLOGD << r1 << ", " << r2 << ", " << fmt::format("offset=0x{:x}", offset);
    return Address((uint16_t)(r1.read() + r2.read() + offset));
  }

  Address _address(const Register &r1, const uint16_t &offset) {
    PLOGD << r1 << ", " << fmt::format("offset=0x{:x}", offset);
    return Address((uint16_t)(r1.read() + offset));
  }

  Address _e0(const uint16_t &offset) {
    return _address(_registers->BX, _registers->SI, offset);
  }

  Address _e1(const uint16_t &offset) {
    return _address(_registers->BX, _registers->DI, offset);
  }

  Address _e2(const uint16_t &offset) {
    return _address(_registers->BP, _registers->SI, offset);
  }

  Address _e3(const uint16_t &offset) {
    return _address(_registers->BP, _registers->DI, offset);
  }

  Address _e4(const uint16_t &offset) {
    return _address(_registers->SI, offset);
  }

  Address _e5(const uint16_t &offset) {
    return _address(_registers->DI, offset);
  }

  Address _e6(const uint16_t &offset) {
    return _address(_registers->BP, offset);
  }

  Address _e7(const uint16_t &offset) {
    return _address(_registers->BX, offset);
  }

public:
  EffectiveAddresser(Registers *registers) : _registers(registers) {
    _eff_mapper = {
        &EffectiveAddresser::_e0, &EffectiveAddresser::_e1,
        &EffectiveAddresser::_e2, &EffectiveAddresser::_e3,
        &EffectiveAddresser::_e4, &EffectiveAddresser::_e5,
        &EffectiveAddresser::_e6, &EffectiveAddresser::_e7,
    };
    _eff_memonics = {
        "[BX]+[SI]", "[BX]+[DI]", "[BP]+[SI]", "[BP]+[DI]",
        "[SI]",      "[DI]",      "[BP]",      "[BX]",
    };
  }
};

class PhysicalAddresser : EffectiveAddresser {
  typedef Address (PhysicalAddresser::*fn_t)(Segment *, const uint16_t &);

protected:
  std::array<fn_t, 8> _phy_mapper;

  Address _f_1(Segment *segment, const uint16_t &eff_addr) {
    PLOGD << *segment << ", "
          << fmt::format("effective_address=0x{:x}", eff_addr);
    return segment->address(eff_addr);
  }

  Address _f0(Segment *segment, const uint16_t &offset) {
    return _f_1(segment, _e0(offset));
  }

  Address _f1(Segment *segment, const uint16_t &offset) {
    return _f_1(segment, _e1(offset));
  }

  Address _f2(Segment *segment, const uint16_t &offset) {
    return _f_1(segment, _e2(offset));
  }

  Address _f3(Segment *segment, const uint16_t &offset) {
    return _f_1(segment, _e3(offset));
  }

  Address _f4(Segment *segment, const uint16_t &offset) {
    return _f_1(segment, _e4(offset));
  }

  Address _f5(Segment *segment, const uint16_t &offset) {
    return _f_1(segment, _e5(offset));
  }

  Address _f6(Segment *segment, const uint16_t &offset) {
    return _f_1(segment, _e6(offset));
  }

  Address _f7(Segment *segment, const uint16_t &offset) {
    return _f_1(segment, _e7(offset));
  }

public:
  PhysicalAddresser(Registers *registers) : EffectiveAddresser(registers) {
    _phy_mapper = {
        &PhysicalAddresser::_f0, &PhysicalAddresser::_f1,
        &PhysicalAddresser::_f2, &PhysicalAddresser::_f3,
        &PhysicalAddresser::_f4, &PhysicalAddresser::_f5,
        &PhysicalAddresser::_f6, &PhysicalAddresser::_f7,
    };
  }

  Address address(Segment *segment, uint8_t idx, uint16_t offset) {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=0x{0:x}, memonic={1}", idx,
                         _eff_memonics[idx]);
    fn_t _map_fn = _phy_mapper[idx];
    return (this->*_map_fn)(segment, offset);
  }

  Address address(Segment *segment, uint8_t idx, uint8_t offset) {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=0x{0:x}, memonic={1}", idx,
                         _eff_memonics[idx]);
    fn_t _map_fn = _phy_mapper[idx];
    return (this->*_map_fn)(segment, offset);
  }

  Address address(Segment *segment, uint8_t idx) {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=0x{0:x}, memonic={1}", idx,
                         _eff_memonics[idx]);
    fn_t _map_fn = _phy_mapper[idx];
    return (this->*_map_fn)(segment, 0x0);
  }

  Address address(Segment *segment, uint16_t offset = 0) {
    PLOGD << "direct: " << fmt::format("offset=0x{:x}", offset);
    fn_t _map_fn = &PhysicalAddresser::_f_1;
    return (this->*_map_fn)(segment, offset);
  }
};

#endif // INSTRUCTION_SET_PHYSICAL_ADDRESSER_H_
