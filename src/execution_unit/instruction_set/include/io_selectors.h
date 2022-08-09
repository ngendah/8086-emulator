//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _IO_SELECTORS_H_
#define _IO_SELECTORS_H_

#include "instruction_templates.h"
#include "logger.h"
#include "physical_addresser.h"
#include "register_mapper.h"
#include "segment_mapper.h"
#include "types.h"

struct IOSelector {
  virtual ~IOSelector() = default;
  virtual IO *get(const Instruction &) = 0;
};

struct RegisterSelector {
  virtual ~RegisterSelector() = default;
  virtual uint8_t REG(const Instruction &instruction) const {
    return instruction.mode_to<opcode_reg_t>().REG;
  }
};

static const auto default_register_selector = RegisterSelector();

class RegisterIOSelector : protected IOSelector {
protected:
  RegisterMapper _registerMapper;
  RegisterSelector const *_selector;

public:
  RegisterIOSelector(Registers *registers, RegisterSelector const *selector =
                                               &default_register_selector)
      : _registerMapper(registers), _selector(selector) {}

  IO *get(const Instruction &instruction) override {
    return _registerMapper.get(_selector->REG(instruction));
  }
};

struct MemorySelector {
  virtual uint8_t MOD(const Instruction &instruction) const = 0;
  virtual uint8_t RM(const Instruction &instruction) const = 0;
  virtual SegmentMappingTypes segment_mapping_type() const = 0;
};

struct DefaultMemorySelector final : MemorySelector {
  uint8_t MOD(const Instruction &instruction) const override {
    mod_reg_rm_t mode = instruction.mode_to<mod_reg_rm_t>();
    PLOGD << mode;
    return mode.MOD;
  }

  uint8_t RM(const Instruction &instruction) const override {
    mod_reg_rm_t mode = instruction.mode_to<mod_reg_rm_t>();
    return mode.RM;
  }

  SegmentMappingTypes segment_mapping_type() const override { return defaults; }
};

static const auto default_memory_selector = DefaultMemorySelector();

struct IOAddresser {
  IOAddresser(Registers *registers, MemorySelector const *selector)
      : _registers(registers), _selector(selector) {}

  template <class T = PhysicalAddresser>
  Address address(const Instruction &instruction) {
    auto _addresser = T(_registers);
    Segment *_segment = segment(instruction, _selector);
    auto MOD = _selector->MOD(instruction);
    auto RM = _selector->RM(instruction);
    if (MOD == 0x0) {
      return _addresser.address(_segment, RM);
    } else if (MOD == 0x1) {
      auto offset = instruction.offset();
      return _addresser.address(_segment, RM, (uint8_t)offset);
    } else if (MOD == 0x2) {
      auto offset = instruction.offset();
      return _addresser.address(_segment, RM, (uint16_t)offset);
    } else {
      PLOGD << "mod=0x3(0b11) is register selector mode";
      assert(false);
    }
  }

  Segment *segment(const Instruction &instruction,
                   MemorySelector const *selector) {
    if (instruction.sop() == 0xff) {
      return SegmentMapper(_registers)
          .get(selector->RM(instruction), selector->segment_mapping_type());
    }
    uint8_t idx = ((instruction.sop() >> 3) & 0x03);
    PLOGD << fmt::format("idx: 0x{:x}", idx);
    return SegmentMapper(_registers).get(idx, indexed);
  }

protected:
  Registers *_registers;
  MemorySelector const *_selector;
};

struct MemoryIOSelector final : IOSelector {
  MemoryIOSelector(BUS *bus, Registers *registers,
                   MemorySelector const *selector = &default_memory_selector)
      : _bus(bus), _registers(registers), _selector(selector) {}

  MemoryIOSelector(const MemoryIOSelector &rhs)
      : _bus(rhs._bus), _registers(rhs._registers), _selector(rhs._selector) {}

  virtual IO *get(const Instruction &instruction) override {
    auto io_addresser = IOAddresser(_registers, _selector);
    _bus.set_address(io_addresser.address<PhysicalAddresser>(instruction));
    PLOGD << _bus;
    return &_bus;
  }

protected:
  BUSIO _bus;
  Registers *_registers;
  MemorySelector const *_selector;
};

struct MemorySegmentSelector : MemorySelector {
  uint8_t _segment_index;
  SegmentMappingTypes _segment_map_type;

  MemorySegmentSelector(uint8_t segment_index = SegmentMapper::DS_INDEX,
                        SegmentMappingTypes segment_map_type = indexed)
      : _segment_index(segment_index), _segment_map_type(segment_map_type) {}

  uint8_t MOD(__attribute__((unused)) const Instruction &) const override {
    assert(false);
    return 0;
  }

  uint8_t RM(__attribute__((unused)) const Instruction &) const override {
    return _segment_index;
  }

  SegmentMappingTypes segment_mapping_type() const override {
    return _segment_map_type;
  }
};

static const auto direct_memory_selector = MemorySegmentSelector();

struct DirectMemoryIOSelector final : IOSelector {
  DirectMemoryIOSelector(
      BUS *bus, Registers *registers,
      MemorySelector const *selector = &direct_memory_selector)
      : _bus(bus), _registers(registers), _selector(selector) {}

  virtual IO *get(const Instruction &instruction) override {
    auto io_addresser = IOAddresser(_registers, _selector);
    Segment *_segment = io_addresser.segment(instruction, _selector);
    uint16_t offset = instruction.offset();
    _bus.set_address(PhysicalAddresser(_registers).address(_segment, offset));
    PLOGD << _bus;
    return &_bus;
  }

protected:
  BUSIO _bus;
  Registers *_registers;
  MemorySelector const *_selector;
};

static const auto stack_memory_segment_selector =
    MemorySegmentSelector(SegmentMapper::SS_INDEX);

struct StackMemoryIOSelector final : IOSelector {
  StackMemoryIOSelector(BUS *bus, Registers *registers)
      : _bus(bus), _registers(registers),
        _selector(&stack_memory_segment_selector) {}

  IO *get(const Instruction &instruction) override {
    Segment *_segment = segment(instruction, _selector);
    _bus.set_address(
        PhysicalAddresser(_registers).address(_segment, _registers->SP));
    return &_bus;
  }

  Segment *segment(const Instruction &instruction,
                   MemorySelector const *selector) {
    return SegmentMapper(_registers)
        .get(selector->RM(instruction), selector->segment_mapping_type());
  }

protected:
  BUSIO _bus;
  Registers *_registers;
  MemorySelector const *_selector;
};

struct SegmentSelector {
  virtual uint8_t SR(const Instruction &instruction) const = 0;
  virtual SegmentMappingTypes map_type() const { return indexed; }
};

struct ModeSegmentSelector : SegmentSelector {
  uint8_t SR(const Instruction &instruction) const override {
    sr_t mode = instruction.mode_to<sr_t>();
    PLOGD << mode;
    return mode.SR;
  }
};

struct OpCodeSegmentSelector : SegmentSelector {
  uint8_t SR(const Instruction &instruction) const override {
    sr_t mode = instruction.opcode_to<sr_t>();
    PLOGD << mode;
    return mode.SR;
  }
};

static const auto default_segment_selector = ModeSegmentSelector();

struct SegmentIOSelector : public IOSelector {
  explicit SegmentIOSelector(
      Registers *registers,
      SegmentSelector const *selector = &default_segment_selector)
      : _segmentMapper(registers), _selector(selector) {}

  IO *get(const Instruction &instruction) override {
    return _segmentMapper.get(_selector->SR(instruction),
                              _selector->map_type());
  }

protected:
  SegmentMapper _segmentMapper;
  SegmentSelector const *_selector;
};

#endif // _IO_SELECTORS_H_
