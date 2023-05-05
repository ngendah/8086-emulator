/*
 * CPU.h
 *
 */

#ifndef CPU_H_
#define CPU_H_

#include "types.h"

#include <cstdint>
#include <string>
#include <vector>

class CPU {
  BUS *_ram;

public:
  CPU(BUS *);
  virtual ~CPU();
  void start();
  void reset();
};

#endif /* CPU_H_ */
