#ifndef INSTRUCTION_SET_SEGMENT_MAPPER_H_
#define INSTRUCTION_SET_SEGMENT_MAPPER_H_

#include <array>

#include "logger.h"
#include "types.h"
#include "./segment_mapping_types.h"

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
    PLOGD << fmt::format("idx: 0x{0:x}, segment-mapping-type: {1}", idx,
                         _SegmentMappingTypes[segment_mapping_type]);
    return segment_mapping_type == indexed
               ? IndexSegmentMapper(_registers).get(idx)
               : DefaultSegmentMapper(_registers).get(idx);
  }

  static const uint8_t ES_INDEX = 0;
  static const uint8_t CS_INDEX = 1;
  static const uint8_t SS_INDEX = 2;
  static const uint8_t DS_INDEX = 3;
};

#endif // INSTRUCTION_SET_SEGMENT_MAPPER_H_
