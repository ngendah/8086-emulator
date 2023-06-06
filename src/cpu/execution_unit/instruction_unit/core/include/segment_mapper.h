//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef INSTRUCTION_SET_SEGMENT_MAPPER_H_
#define INSTRUCTION_SET_SEGMENT_MAPPER_H_

#include "./segment_mapping_types.h"
#include "logger.h"
#include "types.h"

#include <array>

class SegmentMapper {
protected:
  struct Mapper {
    virtual ~Mapper() = default;
    virtual Segment *get(uint8_t idx) = 0;
  };

  struct DefaultSegmentMapper final : Mapper {
    std::array<Segment *, 8> _segment_mapper = {};
    DefaultSegmentMapper(Registers *registers) {
      _segment_mapper = {
          &registers->DS, // AX => DS
          &registers->DS, // CX => DS
          &registers->SS, // DX => SS
          &registers->SS, // BX => SS
          &registers->DS, // SP => DS
          &registers->DS, // BP => DS
          &registers->SS, // SI => SS
          &registers->SS, // DI => DS
      };
    }

    Segment *get(uint8_t idx) override { return _segment_mapper.at(idx); }
  };

  struct IndexSegmentMapper final : Mapper {
    std::array<Segment *, 8> _segment_mapper;
    IndexSegmentMapper(Registers *registers)
        : _segment_mapper({
              &registers->ES,
              &registers->CS,
              &registers->SS,
              &registers->DS,
          }) {}

    Segment *get(uint8_t idx) { return _segment_mapper.at(idx); }
  };

  Registers *_registers;

public:
  SegmentMapper(Registers *registers) : _registers(registers) {}

  Segment *get(uint8_t idx, SegmentMappingTypes segment_mapping_type) {
    auto _segment = segment_mapping_type == indexed
                        ? IndexSegmentMapper(_registers).get(idx)
                        : DefaultSegmentMapper(_registers).get(idx);
    PLOGD << fmt::format("idx=0x{0:x}, "
                         "segment_mapping_type={1}, name={2}, ptr=0x{3:x}",
                         idx, _SegmentMappingTypes[segment_mapping_type],
                         _segment->name(), (long)_segment);
    return _segment;
  }

  static const uint8_t ES_INDEX = 0;
  static const uint8_t CS_INDEX = 1;
  static const uint8_t SS_INDEX = 2;
  static const uint8_t DS_INDEX = 3;

  static const uint8_t SOP_ES_INDEX = 0;
  static const uint8_t SOP_CS_INDEX = (uint8_t)(1 << 3);
  static const uint8_t SOP_SS_INDEX = (uint8_t)(2 << 3);
  static const uint8_t SOP_DS_INDEX = (uint8_t)(3 << 3);
};

#endif // INSTRUCTION_SET_SEGMENT_MAPPER_H_
