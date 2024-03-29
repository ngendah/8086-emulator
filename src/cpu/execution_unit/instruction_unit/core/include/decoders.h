//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _DECODERS_H_
#define _DECODERS_H_

#include "address.h"
#include "bus.h"
#include "decoder.h"
#include "instruction_templates.h"
#include "io_selectors.h" // TODO fix relative path
#include "mov_operators.h"
#include "types.h"
#include "value_io.h"

#include <memory>

// Register-Register move decoder
struct RR_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct RegisterSelector2 : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.mode_to<mod_reg_rm_t>().RM;
    }
  };

  RR_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers),
        _selectors({std::make_unique<RegisterSelector1>(),
                    std::make_unique<RegisterSelector2>()}) {}

  IO *source(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    return RegisterIOSelector(_registers, &(*_selectors.at(opcode.D)))
        .get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
    // "reverse" the array by scaling
    return RegisterIOSelector(_registers, &(*_selectors.at((opcode.D + 1) % 2)))
        .get(instruction);
  }

protected:
  std::array<std::unique_ptr<RegisterSelector>, 2> _selectors;
};

// Register-Register move decoder for increment/decrement
struct DRR_Decoder : Decoder {
  struct RegisterSelector1 final : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &instruction) const override {
      return instruction.opcode_to<opcode_reg_t>().REG;
    }
  };

  DRR_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &selector).get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &selector).get(instruction);
  }
};

// Register-Memory move decoder
struct RM_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  RM_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io_selector(bus, registers) {}

  IO *register_selector(const Instruction &instruction) {
    auto register_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

  IO *memory_selector(const Instruction &instruction) {
    return _io_selector.get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    auto opcode = instruction.opcode_to<d_w_t>();
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

// Memory-Memory move decoder for increment/decrement
struct DMM_Decoder final : Decoder {
  DMM_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io_selector(bus, registers) {}

  IO *memory_selector(const Instruction &instruction) {
    return _io_selector.get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    return memory_selector(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    return memory_selector(instruction);
  }

protected:
  MemoryIOSelector _io_selector;
};

// Register-Immediate move decoder
struct RI_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      auto _opcode = instruction.opcode_to<opcode_w_reg_t>();
      return _opcode.REG;
    }
    uint8_t W(const Instruction &instruction) const override {
      auto _opcode = instruction.opcode_to<opcode_w_reg_t>();
      return _opcode.W;
    }
  };

  RI_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto w_reg = instruction.opcode_to<opcode_w_reg_t>();
    if (w_reg.W == 1) {
      _value_io = instruction.data<uint16_t>();
    } else {
      _value_io = instruction.data<uint8_t>();
    }
    return &_value_io;
  }

  IO *destination(const Instruction &instruction) override {
    auto _selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &_selector).get(instruction);
  }

protected:
  ValueIO _value_io;
};

// Register-Immediate compare decoder
struct CMP_RI_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.mode_to<mod_rm_t>().RM;
    }
  };

  CMP_RI_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto s_w = instruction.opcode_to<s_w_t>();
    if (s_w.W == 1) {
      _value_io = instruction.data<uint16_t>();
    } else {
      _value_io = instruction.data<uint8_t>();
    }
    return &_value_io;
  }

  IO *destination(const Instruction &instruction) override {
    auto _selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &_selector).get(instruction);
  }

protected:
  ValueIO _value_io;
};

// Register-Segment move decoder
struct RS_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.mode_to<mod_sr_rm_t>().RM;
    }
  };

  RS_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *register_selector(const Instruction &instruction) {
    auto register_selector = RegisterSelector1();
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

// Memory-Segment move decoder
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

// Memory-Accumulator move decoder
struct MA_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::AX_INDEX;
    }
  };

  MA_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io_selector(bus, registers) {}

  IO *register_selector(const Instruction &instruction) {
    auto register_selector = RegisterSelector1();
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

// Memory-Immediate move decoder
struct MI_Decoder final : Decoder {
  MI_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io_selector(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto opcode_w = instruction.opcode_to<opcode_w_t>();
    if (opcode_w.W == 1) {
      _value_io = instruction.data<uint16_t>();
    } else {
      _value_io = instruction.data<uint8_t>();
    }
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

// Accumulator - [AX][BX] memory translate decoder
struct AXMXLAT_Decoder final : Decoder {
  AXMXLAT_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io(bus, registers, &_selector) {}

  struct RegisterSelector1 final : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct MemorySelector1 final : MemorySelector {
    uint8_t MOD(UNUSED_PARAM const Instruction &) const override { return 0x2; }
    uint8_t RM(UNUSED_PARAM const Instruction &) const override {
      return PhysicalAddresser::AX_BX_INDEX;
    }
    SegmentMappingTypes segment_mapping_type() const override {
      return defaults;
    }
  };

  IO *source(const Instruction &instruction) override {
    return _io.get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto register_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

protected:
  MemoryIOSelector _io;
  MemorySelector1 _selector;
};

// Accumulator - Register exchange decoder
struct AXRXCH_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::AX_INDEX;
    }
  };

  struct RegisterSelector2 : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.opcode_to<opcode_reg_t>().REG;
    }
  };

  AXRXCH_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto reg_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &reg_selector).get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto reg_selector = RegisterSelector2();
    return RegisterIOSelector(_registers, &reg_selector).get(instruction);
  }
};

// Accumulator
struct CMP_AXI_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::AX_INDEX;
    }
  };

  CMP_AXI_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto _selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &_selector).get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto s_w = instruction.opcode_to<s_w_t>();
    if (s_w.W == 1) {
      _value_io = instruction.data<uint16_t>();
    } else {
      _value_io = instruction.data<uint8_t>();
    }
    return &_value_io;
  }

protected:
  ValueIO _value_io;
};

// Register-Memory exchange decoder
struct RMXCH_Decoder final : Decoder {
  struct RegisterSelector1 final : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  struct RegisterSelector2 final : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.mode_to<mod_reg_rm_t>().RM;
    }
  };

  RMXCH_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _io_selector(bus, registers) {}

  IO *memory_selector(const Instruction &instruction) {
    return _io_selector.get(instruction);
  }

  IO *register_selector_1(const Instruction &instruction) {
    auto register_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    return mode.MOD == 0x3 ? register_selector_1(instruction)
                           : memory_selector(instruction);
  }

  IO *register_selector_2(const Instruction &instruction) {
    auto register_selector = RegisterSelector2();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    return mode.MOD == 0x3 ? register_selector_2(instruction)
                           : memory_selector(instruction);
  }

protected:
  MemoryIOSelector _io_selector;
};

// Register-Stack push decoder
struct RSTK_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      auto mode = instruction.opcode_to<opcode_reg_t>();
      return mode.REG;
    }
  };

  RSTK_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers) {}

  IO *destination(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    auto reg_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &reg_selector).get(instruction);
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
};

// IP (Instruction-Pointer) Register-Stack push decoder
struct IPSTK_Decoder final : Decoder {
  IPSTK_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers) {}

  IO *destination(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *source(UNUSED_PARAM const Instruction &) override {
    return &_registers->IP;
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
};

// Memory-Stack push decoder
struct MSTK_Decoder final : Decoder {
  MSTK_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers),
        _mem_selector(bus, registers) {}

  IO *destination(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    assert(mode.REG == 0x6);
    return _mem_selector.get(instruction);
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
  MemoryIOSelector _mem_selector;
};

// Segment-Stack push decoder
struct SSTK_Decoder final : Decoder {
  SSTK_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers) {}

  IO *destination(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *source(const Instruction &instruction) override {
    auto seg_selector = OpCodeSegmentSelector();
    return SegmentIOSelector(_registers, &seg_selector).get(instruction);
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
};

// Flags-Stack push decoder
struct FSTK_Decoder final : Decoder {
  FSTK_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers) {}

  IO *destination(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *source(UNUSED_PARAM const Instruction &) override {
    return &_registers->FLAGS;
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
};

// Stack-Register pop decoder
struct STKR_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      auto mode = instruction.opcode_to<opcode_reg_t>();
      return mode.REG;
    }
  };

  STKR_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto reg_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &reg_selector).get(instruction);
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
};

// Stack - IP(InstructionPointer) Register pop decoder
struct STKIP_Decoder final : Decoder {
  STKIP_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *destination(UNUSED_PARAM const Instruction &) override {
    return &_registers->IP;
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
};

// Stack-Memory pop decoder
struct STKM_Decoder final : Decoder {

  STKM_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers),
        _mem_selector(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto mode = instruction.mode_to<mod_reg_rm_t>();
    assert(mode.REG == 0x0);
    return _mem_selector.get(instruction);
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
  MemoryIOSelector _mem_selector;
};

// Stack-Segment pop decoder
struct STKS_Decoder final : Decoder {
  STKS_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    auto seg_selector = OpCodeSegmentSelector();
    return SegmentIOSelector(_registers, &seg_selector).get(instruction);
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
};

// Stack-Flags pop decoder
struct STKF_Decoder final : Decoder {
  STKF_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers), _stack_mem_selector(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    return _stack_mem_selector.get(instruction);
  }

  IO *destination(UNUSED_PARAM const Instruction &) override {
    return &_registers->FLAGS;
  }

protected:
  StackMemoryIOSelector _stack_mem_selector;
};

// AX-Port number out decoder
struct AXO_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::AX_INDEX;
    }
  };

  AXO_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto register_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

  IO *destination(const Instruction &instruction) override {
    _port.device(_bus->io(instruction.port()));
    return &_port;
  }

protected:
  PortIO _port;
};

// AX-DX out decoder
struct AXDX_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::AX_INDEX;
    }
  };

  AXDX_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto register_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

  IO *destination(UNUSED_PARAM const Instruction &) override {
    _port.device(_bus->io(_registers->DX));
    return &_port;
  }

protected:
  PortIO _port;
};

// Port-AX in decoder
struct IAX_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::AX_INDEX;
    }
  };

  IAX_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    _port.device(_bus->io(instruction.port()));
    return &_port;
  }

  IO *destination(const Instruction &instruction) override {
    auto register_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

protected:
  PortIO _port;
};

// DX-AX in decoder
struct IAXDX_Decoder final : Decoder {
  struct RegisterSelector1 : RegisterSelector {
    uint8_t REG(UNUSED_PARAM const Instruction &) const override {
      return RegisterMapper::AX_INDEX;
    }
  };

  IAXDX_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(UNUSED_PARAM const Instruction &) override {
    _port.device(_bus->io(_registers->DX));
    return &_port;
  }

  IO *destination(const Instruction &instruction) override {
    auto register_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

protected:
  PortIO _port;
};

// Effective Address - Register decoder
struct REA_Decoder : Decoder {
  struct RegisterSelector1 final : RegisterSelector {
    uint8_t REG(const Instruction &instruction) const override {
      return instruction.mode_to<mod_reg_rm_t>().REG;
    }
  };

  REA_Decoder(bus_ptr_t bus, registers_ptr_t registers)
      : Decoder(bus, registers) {}

  IO *source(const Instruction &instruction) override {
    auto io_addresser = IOAddresser(_registers, &default_memory_selector);
    auto address = io_addresser.address<EffectiveAddresser>(instruction);
    _io.write(cast_ui16(address));
    return &_io;
  }

  IO *destination(const Instruction &instruction) override {
    auto register_selector = RegisterSelector1();
    return RegisterIOSelector(_registers, &register_selector).get(instruction);
  }

protected:
  ValueIO _io;
};

#endif // _DECODERS_H_
