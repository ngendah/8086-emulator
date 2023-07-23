#include "cpu.h"

#include <fstream>

CPU::CPU(BUS *bus) : _execution_unit(bus, &_registers), _power_off(false) {}

CPU::~CPU() {}

void CPU::bootstrap(const std::string &program) {
  auto ostream = std::ofstream{program, std::ios::binary};
  _execution_unit.bootstrap(ostream.rdbuf());
}

bool CPU::power_off() const { return _power_off; }

void CPU::execute() { _execution_unit.fetch_decode_execute(); }
