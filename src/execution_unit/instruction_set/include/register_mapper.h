//
// Copyright (c) 2022 Ngenda Henry
// All rights reserved.
//
// For the license information refer to LICENSE.

#ifndef INSTRUCTION_SET_REGISTER_MAPPER_H_
#define INSTRUCTION_SET_REGISTER_MAPPER_H_

#include <array>
#include <functional>

#include "types.h"

class RegisterMapper {
  /**
   * Combination of W and REG fields to select registers,
   * "->" is used to indicate the index of the array its mapped to
   *
   * W REG     Windows
   * ---------------
   * 0 000 AL->0
   * 0 001 CL->1
   * 0 010 DL->2
   * 0 011 BL->3
   *
   * 0 100 AH->0
   * 0 101 CH->1
   * 0 110 DH->2
   * 0 111 BH->3
   *
   * 1 000 AX->0
   * 1 001 CX->1
   * 1 010 DX->2
   * 1 011 BX->3
   * 1 100 SP->4
   * 1 101 BP->5
   * 1 110 SI->6
   * 1 111 DI->7
   *
   */
  std::array<Register *, 9> _mapper{};

public:
  RegisterMapper(Registers *registers) {
    _mapper = {
        &registers->AX, &registers->CX, &registers->DX, &registers->BX,
        &registers->SP, &registers->BP, &registers->SI, &registers->DI,
    };
  }

  virtual ~RegisterMapper() = default;

  virtual Register *get(const uint8_t idx) {
    // slide the index to its appropriate "window"
    // as shown on the table above
    auto _idx = idx <= 7 ? idx % 4 : idx % 8;
    auto _register = _mapper.at(_idx);
    PLOGD << fmt::format("idx=0x{0:x}, name={1}, ptr=0x{2:x}", _idx,
                         _register->name(), (long)_register); // NOLINT
    return _register;
  }

  /**
   *
   * Register selection will depend on whether the operation
   * is on a word or byte data.
   *
   * In order to select a register for a word operation
   * the REG bits are prepended with a 1-bit as the MSB,
   * while for byte operation its prepended with a 0-bit.
   *
   * The selection of which value to prepend depends on the W bit
   * of the opcode.
   *
   * Since this is a 16-byte processor the default action is to prepend
   * 1-bit as the MSB.
   *
   * For details on REG and W bits, refer to instruction_templates.h
   */
  static uint8_t to_idx(uint8_t w, uint8_t reg) {
    return (uint8_t)((w << 3) | reg);
  }

  static const uint8_t AX_INDEX = 0;
  static const uint8_t CX_INDEX = 1;
  static const uint8_t DX_INDEX = 2;
  static const uint8_t BX_INDEX = 3;
  static const uint8_t SP_INDEX = 4;
  static const uint8_t BP_INDEX = 5;
  static const uint8_t SI_INDEX = 6;
  // custom extensions
  static const uint8_t DI_INDEX = 7;
};

#endif // INSTRUCTION_SET_REGISTER_MAPPER_H_
