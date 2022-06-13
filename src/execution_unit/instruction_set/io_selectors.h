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
  virtual uint8_t REG(const Instruction &instruction) {
    return instruction.mode_to<opcode_reg_t>().REG;
  }
};

class RegisterIOSelector : protected IOSelector {
protected:
  RegisterMapper _registerMapper;
  RegisterSelector *_selector;

public:
  explicit RegisterIOSelector(Registers *registers,
                              RegisterSelector *selector = nullptr)
      : _registerMapper(registers), _selector(selector) {}

  IO *get(const Instruction &instruction) override {
    if (_selector != nullptr)
      return _registerMapper.get(_selector->REG(instruction));
    auto selector = RegisterSelector();
    return _registerMapper.get(selector.REG(instruction));
  }
};

struct MemorySelector {
  virtual uint8_t MOD(const Instruction &instruction) {
    mod_reg_rm_t mode = instruction.mode_to<mod_reg_rm_t>();
    PLOGD << mode;
    return mode.MOD;
  }

  virtual uint8_t RM(const Instruction &instruction) {
    mod_reg_rm_t mode = instruction.mode_to<mod_reg_rm_t>();
    PLOGD << mode;
    return mode.RM;
  }

  virtual SegmentMappingTypes segment_mapping_type() { return defaults; }
};

class MemoryIOSelector : protected IOSelector {
protected:
  BUSIO _bus;
  Registers *_registers;
  MemorySelector *_selector;

public:
  MemoryIOSelector(BUS *bus = nullptr, Registers *registers = nullptr,
                   MemorySelector *selector = nullptr)
      : _bus(bus), _registers(registers), _selector(selector) {}

  MemoryIOSelector(const MemoryIOSelector &rhs)
      : _bus(rhs._bus), _registers(rhs._registers) {}

  IO *get(const Instruction &instruction) override {
    MemorySelector *selector = this->_selector;
    auto def_selector = MemorySelector();
    if (selector == nullptr)
      selector = &def_selector;
    Segment *_segment = segment(instruction, selector);
    auto MOD = selector->MOD(instruction);
    auto RM = selector->RM(instruction);
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

  Segment *segment(const Instruction &instruction, MemorySelector *selector) {
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
  virtual uint8_t SR(const Instruction &instruction) = 0;
};

struct ModeSegmentSelector : SegmentSelector {
  uint8_t SR(const Instruction &instruction) override {
    sr_t mode = instruction.mode_to<sr_t>();
    PLOGD << mode;
    return mode.SR;
  }
};

struct OpCodeSegmentSelector : SegmentSelector {
  uint8_t SR(const Instruction &instruction) override {
    sr_t mode = instruction.opcode_to<sr_t>();
    PLOGD << mode;
    return mode.SR;
  }
};

class SegmentIOSelector : public IOSelector {
protected:
  SegmentMapper _segmentMapper;
  SegmentSelector *_selector;

public:
  explicit SegmentIOSelector(Registers *registers,
                             SegmentSelector *selector = nullptr)
      : _segmentMapper(registers), _selector(selector) {}

  IO *get(const Instruction &instruction) override {
    if (_selector != nullptr)
      return _segmentMapper.get(this->_selector->SR(instruction), indexed);
    auto selector = OpCodeSegmentSelector();
    return _segmentMapper.get(selector.SR(instruction), indexed);
  }
};

#endif // _IO_SELECTORS_H_
