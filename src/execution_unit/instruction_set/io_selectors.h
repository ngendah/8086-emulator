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
  explicit RegisterIOSelector(
      Registers *registers,
      RegisterSelector const *selector = &default_register_selector)
      : _registerMapper(registers), _selector(selector) {}

  IO *get(const Instruction &instruction) override {
    return _registerMapper.get(_selector->REG(instruction));
  }
};

struct MemorySelector {
  virtual uint8_t MOD(const Instruction &instruction) const {
    mod_reg_rm_t mode = instruction.mode_to<mod_reg_rm_t>();
    PLOGD << mode;
    return mode.MOD;
  }

  virtual uint8_t RM(const Instruction &instruction) const {
    mod_reg_rm_t mode = instruction.mode_to<mod_reg_rm_t>();
    PLOGD << mode;
    return mode.RM;
  }

  virtual SegmentMappingTypes segment_mapping_type() const { return defaults; }
};

static const auto default_memory_selector = MemorySelector();

class MemoryIOSelector : protected IOSelector {
protected:
  BUSIO _bus;
  Registers *_registers;
  MemorySelector const *_selector;

public:
  MemoryIOSelector(BUS *bus, Registers *registers,
                   MemorySelector const *selector = &default_memory_selector)
      : _bus(bus), _registers(registers), _selector(selector) {}

  MemoryIOSelector(const MemoryIOSelector &rhs)
      : _bus(rhs._bus), _registers(rhs._registers), _selector(rhs._selector) {}

  IO *get(const Instruction &instruction) override {
    Segment *_segment = segment(instruction, _selector);
    auto MOD = _selector->MOD(instruction);
    auto RM = _selector->RM(instruction);
    if (MOD == 0x0) {
      _bus.set_address(PhysicalAddresser(_registers).address(_segment, RM));
    } else if (MOD == 0x1) {
      auto offset = instruction.offset();
      _bus.set_address(
          PhysicalAddresser(_registers).address(_segment, RM, (uint8_t)offset));
    } else if (MOD == 0x2) {
      auto offset = instruction.offset();
      _bus.set_address(PhysicalAddresser(_registers)
                           .address(_segment, RM, (uint16_t)offset));
    } else {
      PLOGD << "mod=0x3(0b11) is register selector mode";
      assert(false);
    }
    return &_bus;
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
};

struct SegmentSelector {
  virtual uint8_t SR(const Instruction &instruction) const = 0;
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

static const auto default_segment_selector = OpCodeSegmentSelector();

class SegmentIOSelector : public IOSelector {
protected:
  SegmentMapper _segmentMapper;
  SegmentSelector const *_selector;

public:
  explicit SegmentIOSelector(
      Registers *registers,
      SegmentSelector const *selector = &default_segment_selector)
      : _segmentMapper(registers), _selector(selector) {}

  IO *get(const Instruction &instruction) override {
    return _segmentMapper.get(_selector->SR(instruction), indexed);
  }
};

#endif // _IO_SELECTORS_H_
