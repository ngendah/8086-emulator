//
// Copyright (c) 2022 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef INSTRUCTION_SET_SEGMENT_MAPPER_H_
#define INSTRUCTION_SET_SEGMENT_MAPPER_H_

#include <array>

#include "./segment_mapping_types.h"
#include "logger.h"
#include "types.h"

class SegmentMapper {
protected:
  struct DefaultSegmentMapper {
    std::array<Segment *, 4> _segment_mapper = {};
    DefaultSegmentMapper(Registers *registers) {
      _segment_mapper = {
          &registers->ES,
          &registers->CS,
          &registers->SS,
          &registers->DS,
      };
    }

    virtual Segment *get(uint8_t idx) {
      std::array<Segment *, 8> _mapping = {
          _segment_mapper[3], // AX => DS
          _segment_mapper[3], // CX => DS
          _segment_mapper[2], // DX => SS
          _segment_mapper[2], // BX => SS
          _segment_mapper[3], // SP => DS
          _segment_mapper[3], // BP => DS
          _segment_mapper[2], // SI => SS
          _segment_mapper[2], // DI => DS
      };
      return _mapping[idx];
    }
  };

  class IndexSegmentMapper : protected DefaultSegmentMapper {
  public:
    IndexSegmentMapper(Registers *registers)
        : DefaultSegmentMapper(registers) {}

    Segment *get(uint8_t idx) { return _segment_mapper[idx]; }
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
