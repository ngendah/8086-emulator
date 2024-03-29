//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _IO_SELECTORS_H_
#define _IO_SELECTORS_H_

#include "address.h"
#include "bus.h"
#include "bus_io.h"
#include "instruction.h"
#include "instruction_templates.h"
#include "logger.h"
#include "physical_addresser.h"
#include "register_mapper.h"
#include "segment_mapper.h"
#include "sop.h"
#include "types.h"

struct IOSelector {
  virtual ~IOSelector() = default;
  virtual IO *get(const Instruction &) = 0;
};

/**
 *
 * For how registers are selected refer to register_mapper.h
 */
struct RegisterSelector {
  virtual ~RegisterSelector() = default;
  virtual uint8_t REG(const Instruction &instruction) const {
    return instruction.mode_to<opcode_reg_t>().REG;
  }
  virtual uint8_t W(const Instruction &) const { return 1; }
};

static const auto default_register_selector = RegisterSelector();

struct RegisterIOSelector : IOSelector {
  RegisterIOSelector(Registers *registers, RegisterSelector const *selector =
                                               &default_register_selector)
      : _registerMapper(registers), _selector(selector) {}

  IO *get(const Instruction &instruction) override {
    auto _idx = RegisterMapper::to_idx(_selector->W(instruction),
                                       _selector->REG(instruction));
    auto *_register = _registerMapper.get(_idx);
    return _register;
  }

protected:
  RegisterMapper _registerMapper;
  RegisterSelector const *_selector;
};

struct MemorySelector {
  virtual ~MemorySelector() = default;
  virtual uint8_t MOD(const Instruction &instruction) const = 0;
  virtual uint8_t RM(const Instruction &instruction) const = 0;
  virtual SegmentMappingTypes segment_mapping_type() const = 0;
};

struct DefaultMemorySelector : MemorySelector {
  uint8_t MOD(const Instruction &instruction) const override {
    mod_reg_rm_t mode = instruction.mode_to<mod_reg_rm_t>();
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
    auto MOD = _selector->MOD(instruction);
    auto RM = _selector->RM(instruction);
    auto offset = instruction.offset();
    if (MOD == AddressingModes::MOD::MEM_NO_DISPLACEMENT) {
      if (RM == AddressingModes::RM::DIRECT_ADDRESSING) {
        auto _segment = direct_addressing_segment(instruction);
        return _addresser.address(_segment, cast_ui16(offset));
      }
      auto *_segment = segment(instruction, _selector);
      return _addresser.address(_segment, RM);
    }
    if (MOD == AddressingModes::MOD::MEM_DISPLACEMENT_8) {
      auto *_segment = segment(instruction, _selector);
      return _addresser.address(_segment, RM, cast_ui8(offset));
    }
    if (MOD == AddressingModes::MOD::MEM_DISPLACEMENT_16) {
      auto *_segment = segment(instruction, _selector);
      return _addresser.address(_segment, RM, cast_ui16(offset));
    }
    PLOGD << "mod=0x3(0b11) is register selector mode";
    assert(false);
  }

  Segment *segment(const Instruction &instruction,
                   MemorySelector const *selector) {
    if (instruction.sop() == SOP::NONE) {
      return SegmentMapper(_registers)
          .get(selector->RM(instruction), selector->segment_mapping_type());
    }
    uint8_t idx = ((instruction.sop() >> 3) & 0x03);
    PLOGD << fmt::format("idx: 0x{:x}", idx);
    return SegmentMapper(_registers).get(idx, indexed);
  }

  Segment *direct_addressing_segment(const Instruction &instruction) {
    if (instruction.sop() == SOP::NONE) {
      return SegmentMapper(_registers).get(SegmentMapper::DS_INDEX, indexed);
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
      : _addr_latch(bus), _registers(registers), _selector(selector) {}

  MemoryIOSelector(const MemoryIOSelector &rhs)
      : _addr_latch(rhs._addr_latch), _registers(rhs._registers),
        _selector(rhs._selector) {}

  IO *get(const Instruction &instruction) override {
    auto io_addresser = IOAddresser(_registers, _selector);
    _addr_latch.set_address(
        io_addresser.address<PhysicalAddresser>(instruction));
    return &_addr_latch;
  }

protected:
  AddressLatch _addr_latch;
  Registers *_registers;
  MemorySelector const *_selector;
};

struct MemorySegmentSelector : MemorySelector {
  uint8_t _segment_index;
  SegmentMappingTypes _segment_map_type;

  MemorySegmentSelector(const uint8_t segment_index = SegmentMapper::DS_INDEX,
                        SegmentMappingTypes segment_map_type = indexed)
      : _segment_index(segment_index), _segment_map_type(segment_map_type) {}

  uint8_t MOD(UNUSED_PARAM const Instruction &) const override {
    assert(false);
    return 0;
  }

  uint8_t RM(UNUSED_PARAM const Instruction &) const override {
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
      : _addr_latch(bus), _registers(registers), _selector(selector) {}

  IO *get(const Instruction &instruction) override {
    auto io_addresser = IOAddresser(_registers, _selector);
    Segment *_segment = io_addresser.segment(instruction, _selector);
    uint16_t offset = instruction.offset();
    _addr_latch.set_address(
        PhysicalAddresser(_registers).address(_segment, offset));
    PLOGD << _addr_latch;
    return &_addr_latch;
  }

protected:
  AddressLatch _addr_latch;
  Registers *_registers;
  MemorySelector const *_selector;
};

static const auto stack_memory_segment_selector =
    MemorySegmentSelector(SegmentMapper::SS_INDEX);

struct StackMemoryIOSelector final : IOSelector {
  StackMemoryIOSelector(BUS *bus, Registers *registers)
      : _addr_latch(bus), _registers(registers),
        _selector(&stack_memory_segment_selector) {}

  IO *get(const Instruction &instruction) override {
    Segment *_segment = segment(instruction, _selector);
    auto address =
        PhysicalAddresser(_registers).address(_segment, _registers->SP);
    _addr_latch.set_address(address);
    PLOGD << fmt::format("AddressLatch object ptr=0x{:x}", (long)&_addr_latch);
    return &_addr_latch;
  }

  Segment *segment(const Instruction &instruction,
                   MemorySelector const *selector) {
    return SegmentMapper(_registers)
        .get(selector->RM(instruction), selector->segment_mapping_type());
  }

protected:
  AddressLatch _addr_latch;
  Registers *_registers;
  MemorySelector const *_selector;
};

struct SegmentSelector {
  virtual ~SegmentSelector() = default;
  virtual uint8_t SR(const Instruction &instruction) const = 0;
  virtual SegmentMappingTypes map_type() const { return indexed; }
};

struct ModeSegmentSelector : SegmentSelector {
  ~ModeSegmentSelector() override = default;
  uint8_t SR(const Instruction &instruction) const override {
    sr_t mode = instruction.mode_to<sr_t>();
    return mode.SR;
  }
};

struct OpCodeSegmentSelector : SegmentSelector {
  uint8_t SR(const Instruction &instruction) const override {
    sr_t mode = instruction.opcode_to<sr_t>();
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
    auto _segment =
        _segmentMapper.get(_selector->SR(instruction), _selector->map_type());
    return _segment;
  }

protected:
  SegmentMapper _segmentMapper;
  SegmentSelector const *_selector;
};

#endif // _IO_SELECTORS_H_
