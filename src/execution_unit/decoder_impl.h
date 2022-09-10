/*
 * InstructionDecoder.h
 *
 */

#ifndef INSTRUCTION_DECODER_H_
#define INSTRUCTION_DECODER_H_

#include "types.h"

#include <cstdint>
#include <vector>

class DecoderImpl : public Decoder {
  Registers *_registers;
  BUS *_bus;

public:
  DecoderImpl(BUS *, Registers *);
  virtual ~DecoderImpl();
  void decodeAndExecute();

protected:
  uint16_t fetch(uint16_t);
};

#endif /* INSTRUCTION_DECODER_H_ */
