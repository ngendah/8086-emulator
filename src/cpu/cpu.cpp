#include "cpu.h"

CPU::CPU(BUS *ram) : _execution_unit(ram, &_registers), _power_off(false) {}

CPU::~CPU() {}

void CPU::bootstrap(const std::string &os,
                    const std::vector<Device *> &devices) {
  _power_off = false;
  (void)os; // load program
  for (auto *device : devices) {
    device->bootstrap(&_registers.PORTS,
                      dynamic_cast<InterruptHandler *>(this));
  }
}

bool CPU::power_off() const { return _power_off; }

void CPU::execute() { _execution_unit.fetch_decode_execute(); }

void CPU::interrupt(uint8_t type) {
  if (type == 0x19) {
    _power_off = true;
    return;
  }
  _execution_unit.interrupt(type);
}
