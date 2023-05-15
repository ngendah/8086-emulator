//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef CPU_H_
#define CPU_H_

#include "device.h"
#include "execution_unit.h"

#include <vector>

class CPU final : InterruptHandler {
  Registers _registers;
  ExecutionUnit _execution_unit;
  bool _halt;

public:
  CPU(BUS *ram);
  ~CPU() override;
  void bootstrap(const std::string &, const std::vector<Device *> &devices);
  bool halt() const;
  void power_off();
  void execute();

protected:
  void interrupt(uint8_t type) override;
};

#endif /* CPU_H_ */
