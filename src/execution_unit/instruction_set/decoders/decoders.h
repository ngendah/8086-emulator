//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _DECODERS_H_
#define _DECODERS_H_

#include "instruction_templates.h"
#include "io_selectors.h"
#include "mov_operators.h"
#include "types.h"

struct RR_Decoder final : Decoder {
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct _RegisterSelector2 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().RM;
    }
  };

  RR_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers),
        _selectors({std::make_unique<_RegisterSelector1>(),
                    std::make_unique<_RegisterSelector2>()}) {}

  IO *source(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    return RegisterIOSelector(_registers, &(*_selectors[opcode.D]))
        .get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    // "reverse" the array by scaling
    return RegisterIOSelector(_registers, &(*_selectors[(opcode.D + 1) % 2]))
        .get(instruction);
  }

protected:
  std::array<std::unique_ptr<RegisterSelector>, 2> _selectors;
};

struct RM_Decoder final : Decoder {
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  RM_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io_selector(bus, registers) {}

  IO *register_selector(const Instruction &instruction) {
    auto register_selector = _RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

  IO *memory_selector(const Instruction &instruction) {
    return _io_selector.get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    assert(opcode.D == 0);
    return opcode.D == 1 ? memory_selector(instruction)
                         : register_selector(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    return opcode.D == 1 ? register_selector(instruction)
                         : memory_selector(instruction);
  }

protected:
  MemoryIOSelector _io_selector;
};

struct RI_Decoder final : Decoder {
  struct _RegisterSelector : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.opcode_to<opcode_w_reg_t>().REG;
    }
  };

  RI_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto w_reg = instruction.opcode_to<opcode_w_reg_t>();
    if (w_reg.W == 1)
      _value_io = instruction.data<uint16_t>();
    else
      _value_io = instruction.data<uint8_t>();
    return &_value_io;
  }

  IO *destination(const Instruction &instruction) override {
    auto _selector = _RegisterSelector();
    return RegisterIOSelector(_registers, &_selector).get(instruction);
  }

protected:
  ValueIO _value_io;
};

struct RS_Decoder final : Decoder {
  struct _RegisterSelector : RegisterSelector {
    virtual uint8_t REG(const Instruction &instruction) const {
      return instruction.mode_to<mod_sr_rm_t>().RM;
    }
  };

  RS_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *register_selector(const Instruction &instruction) {
    auto register_selector = _RegisterSelector();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

  IO *segment_selector(const Instruction &instruction) {
    return SegmentIOSelector(_registers).get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    return opcode.D == 1 ? register_selector(instruction)
                         : segment_selector(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    return opcode.D == 1 ? segment_selector(instruction)
                         : register_selector(instruction);
  }
};

struct MS_Decoder final : Decoder {
  MS_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io_selector(bus, registers) {}

  IO *segment_selector(const Instruction &instruction) {
    return SegmentIOSelector(_registers).get(instruction);
  }

  IO *memory_selector(const Instruction &instruction) {
    return _io_selector.get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    return opcode.D == 1 ? memory_selector(instruction)
                         : segment_selector(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    return opcode.D == 1 ? segment_selector(instruction)
                         : memory_selector(instruction);
  }

protected:
  MemoryIOSelector _io_selector;
};

struct MA_Decoder final : Decoder {
  struct _RegisterSelector1 : RegisterSelector {
    virtual uint8_t REG(UNUSED_PARAM const Instruction &) const {
      return RegisterMapper::AX_INDEX;
    }
  };

  MA_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io_selector(bus, registers) {}

  IO *register_selector(const Instruction &instruction) {
    auto register_selector = _RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

  IO *memory_selector(const Instruction &instruction) {
    return _io_selector.get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    return opcode.D == 1 ? register_selector(instruction)
                         : memory_selector(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    return opcode.D == 1 ? memory_selector(instruction)
                         : register_selector(instruction);
  }

protected:
  DirectMemoryIOSelector _io_selector;
};

struct MI_Decoder final : Decoder {
  MI_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io_selector(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto opcode_w = instruction.opcode_to<opcode_w_t>();
    if (opcode_w.W == 1)
      _value_io = instruction.data<uint16_t>();
    else
      _value_io = instruction.data<uint8_t>();
    return &_value_io;
  }

  IO *destination(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    assert(mode.REG == 0);
    return _io_selector.get(instruction);
  }

protected:
  ValueIO _value_io;
  MemoryIOSelector _io_selector;
};

#endif // _DECODERS_H_
