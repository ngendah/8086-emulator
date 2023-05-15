#include "cpu.h"

CPU::CPU(BUS *ram) : _execution_unit(ram, &_registers), _halt(true) {}

CPU::~CPU() {}

void CPU::bootstrap(const std::string &os,
                    const std::vector<Device *> &devices) {
  _halt = false;
  (void)os; // load program
  for (auto device : devices) {
    device->bootstrap(&_registers.PORTS,
                      dynamic_cast<InterruptHandler *>(this));
  }
}

bool CPU::halt() const { return _halt; }

void CPU::power_off() { _halt = true; }

void CPU::execute() { _execution_unit.fetch_decode_execute(); }

void CPU::interrupt(uint8_t type) { _execution_unit.interrupt(type); }
