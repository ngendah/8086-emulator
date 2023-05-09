//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _EXECUTION_UNIT_H_
#define _EXECUTION_UNIT_H_

#include "instruction_set.h"
#include "micro_op.h"
#include <queue>
#include <streambuf>

struct ExecutionUnit final {
  ExecutionUnit(bus_ptr_t bus, registers_ptr_t registers);

  void fetch_decode_execute();

  void bootstrap(std::streambuf *program, bool replace = false);

  void interrupt(uint8_t type);

  std::pair<uint8_t, Instruction> fetch();

  std::shared_ptr<MicroOp> decode(uint8_t opcode);

  uint16_t beg();

  bool eof() const;

  uint16_t seek(uint16_t pos);

  uint8_t getb();

  uint16_t getw();

protected:
  template <typename T> T _get() {
    T _val = 0;
    _buf->sgetn((char *)&_val, sizeof(_val));
    _params.registers->IP += sizeof(_val);
    return _val;
  }

private:
  InstructionSet _instruction_set;
  std::streambuf *_buf;
  MicroOp::Params _params;
  std::queue<uint8_t> _interrupts;
};

#endif // _EXECUTION_UNIT_H_
