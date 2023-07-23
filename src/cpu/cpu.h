//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef CPU_H_
#define CPU_H_

#include "device.h"
#include "execution_unit.h"

#include <vector>

struct CPU final {
  CPU(BUS *bus);
  ~CPU();
  void bootstrap(const std::string &);
  bool power_off() const;
  void execute();

protected:
  Registers _registers;
  ExecutionUnit _execution_unit;
  bool _power_off;
};

#endif /* CPU_H_ */
