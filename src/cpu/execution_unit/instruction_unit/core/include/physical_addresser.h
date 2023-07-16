//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef INSTRUCTION_SET_PHYSICAL_ADDRESSER_H_
#define INSTRUCTION_SET_PHYSICAL_ADDRESSER_H_

#include "address.h"
#include "bus.h"
#include "logger.h"
#include "registers.h"
#include "types.h"

#include <array>
#include <fmt/core.h>
#include <functional>

struct Addresser {
  virtual ~Addresser() {}
  virtual Address address(Segment *segment, uint8_t idx, uint16_t offset) = 0;
  virtual Address address(Segment *segment, uint8_t idx, uint8_t offset) = 0;
  virtual Address address(Segment *segment, uint8_t idx) = 0;
  virtual Address address(Segment *segment, uint16_t offset) = 0;
};

class EffectiveAddresser : public Addresser {
  typedef Address (EffectiveAddresser::*fn_t)(const uint16_t &);

protected:
  Registers *_registers;
  std::array<fn_t, 8> _eff_mapper;
  std::array<std::string, 8> _eff_memonics;

  static Address _address(const Register &r1, const Register &r2,
                          const uint16_t &offset) {
    PLOGD << r1 << ", " << r2 << ", " << fmt::format("offset=0x{:x}", offset);
    return Address(cast_ui16(r1.read() + r2.read() + offset));
  }

  static Address _address(const Register &r1, const uint16_t &offset) {
    PLOGD << r1 << ", " << fmt::format("offset=0x{:x}", offset);
    return Address(cast_ui16(r1.read() + offset));
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
  EffectiveAddresser(Registers *registers)
      : _registers(registers), _eff_mapper({
                                   &EffectiveAddresser::_e0,
                                   &EffectiveAddresser::_e1,
                                   &EffectiveAddresser::_e2,
                                   &EffectiveAddresser::_e3,
                                   &EffectiveAddresser::_e4,
                                   &EffectiveAddresser::_e5,
                                   &EffectiveAddresser::_e6,
                                   &EffectiveAddresser::_e7,
                               }),
        _eff_memonics({
            "[BX]+[SI]",
            "[BX]+[DI]",
            "[BP]+[SI]",
            "[BP]+[DI]",
            "[SI]",
            "[DI]",
            "[BP]",
            "[BX]",
        }) {}

  Address address(UNUSED_PARAM Segment *, uint8_t idx) override {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=0x{0:x}, memonic={1}", idx,
                         _eff_memonics.at(idx));
    fn_t _map_fn = _eff_mapper.at(idx);
    return (this->*_map_fn)(0x0);
  }

  Address address(UNUSED_PARAM Segment *, uint8_t idx,
                  uint16_t offset) override {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=0x{0:x}, memonic={1}", idx,
                         _eff_memonics.at(idx));
    fn_t _map_fn = _eff_mapper.at(idx);
    return (this->*_map_fn)(offset);
  };

  Address address(UNUSED_PARAM Segment *, uint8_t idx,
                  uint8_t offset) override {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=0x{0:x}, memonic={1}", idx,
                         _eff_memonics.at(idx));
    fn_t _map_fn = _eff_mapper.at(idx);
    return (this->*_map_fn)(offset);
  };

  Address address(Segment *segment, uint16_t offset) override {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=DIRECT, effective_address=0x{:x}",
                         offset);
    return segment->address(offset);
  };

  static const uint8_t BX_SI_INDEX = 0;
  static const uint8_t BX_DI_INDEX = 1;
  static const uint8_t BP_SI_INDEX = 2;
  static const uint8_t BP_DI_INDEX = 3;
  static const uint8_t SI_INDEX = 4;
  static const uint8_t DI_INDEX = 5;
  static const uint8_t BP_INDEX = 6;
  static const uint8_t BX_INDEX = 7;
};

class PhysicalAddresser : public EffectiveAddresser {
  typedef Address (PhysicalAddresser::*fn_t)(Segment *, const uint16_t &);

protected:
  std::array<fn_t, 9> _phy_mapper;
  std::array<std::string, 9> _phy_memonics;

  static Address _f_1(Segment *segment, const uint16_t &eff_addr) {
    PLOGD << *segment << ", "
          << fmt::format("effective_address=0x{:x}", eff_addr);
    return segment->address(eff_addr);
  }

  static Address _f_2(Segment *segment, const Register &register1,
                      const Register &register2) {
    return segment->address(register1, register2.read());
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

  Address _f8(Segment *segment, UNUSED_PARAM const uint16_t &) {
    return _f_2(segment, _registers->AX, _registers->BX);
  }

public:
  PhysicalAddresser(Registers *registers)
      : EffectiveAddresser(registers), _phy_mapper({
                                           &PhysicalAddresser::_f0,
                                           &PhysicalAddresser::_f1,
                                           &PhysicalAddresser::_f2,
                                           &PhysicalAddresser::_f3,
                                           &PhysicalAddresser::_f4,
                                           &PhysicalAddresser::_f5,
                                           &PhysicalAddresser::_f6,
                                           &PhysicalAddresser::_f7,
                                           &PhysicalAddresser::_f8,
                                       }),
        _phy_memonics({
            "[BX]+[SI]",
            "[BX]+[DI]",
            "[BP]+[SI]",
            "[BP]+[DI]",
            "[SI]",
            "[DI]",
            "[BP]",
            "[BX]",
            "[AX][BX]",
        }) {}

  Address address(Segment *segment, uint8_t idx, uint16_t offset) override {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=0x{0:x}, memonic={1}:{2}", idx,
                         segment->name(), _phy_memonics.at(idx));
    fn_t _map_fn = _phy_mapper.at(idx);
    return (this->*_map_fn)(segment, offset);
  }

  Address address(Segment *segment, uint8_t idx, uint8_t offset) override {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=0x{0:x}, memonic={1}:{2}", idx,
                         segment->name(), _phy_memonics.at(idx));
    fn_t _map_fn = _phy_mapper.at(idx);
    return (this->*_map_fn)(segment, offset);
  }

  Address address(Segment *segment, uint8_t idx) override {
    PLOGD << "mapping: "
          << fmt::format("addressing_mode=0x{0:x}, memonic={1}:{2}", idx,
                         segment->name(), _phy_memonics.at(idx));
    fn_t _map_fn = _phy_mapper.at(idx);
    return (this->*_map_fn)(segment, 0x0);
  }

  Address address(Segment *segment, uint16_t offset = 0) override {
    PLOGD << "direct: " << fmt::format("offset=0x{:x}", offset);
    return PhysicalAddresser::_f_1(segment, offset);
  }

  // custom
  static const uint8_t AX_BX_INDEX = 8;
};

struct AddressingModes final {
  struct RM {
    static const uint8_t BX_SI = EffectiveAddresser::BP_SI_INDEX;
    static const uint8_t BX_DI = EffectiveAddresser::BX_DI_INDEX;
    static const uint8_t BP_SI = EffectiveAddresser::BP_SI_INDEX;
    static const uint8_t SI = EffectiveAddresser::SI_INDEX;
    static const uint8_t DI = EffectiveAddresser::DI_INDEX;
    static const uint8_t DIRECT_ADDRESSING = 0x6;
    static const uint8_t BX = EffectiveAddresser::BX_INDEX;
  };
  struct MOD {
    static const uint8_t REG = 0x3;
    static const uint8_t MEM_NO_DISPLACEMENT = 0x0;
    static const uint8_t MEM_DISPLACEMENT_8 = 0x1;
    static const uint8_t MEM_DISPLACEMENT_16 = 0x2;
  };
};

#endif // INSTRUCTION_SET_PHYSICAL_ADDRESSER_H_
