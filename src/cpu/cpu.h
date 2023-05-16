//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef CPU_H_
#define CPU_H_

#include "device.h"
#include "execution_unit.h"

#include <vector>

struct CPU final : InterruptHandler {
  CPU(BUS *ram);
  ~CPU() override;
  void bootstrap(const std::string &, const std::vector<Device *> &devices);
  bool power_off() const;
  void execute();

protected:
  void interrupt(uint8_t type) override;

  Registers _registers;
  ExecutionUnit _execution_unit;
  bool _power_off;
};

#endif /* CPU_H_ */
