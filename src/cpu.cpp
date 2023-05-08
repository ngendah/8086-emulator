#include "cpu.h"

CPU::CPU(BUS *ram) : _execution_unit(ram, &_registers), _halt(true) {}

CPU::~CPU() {}

void CPU::bootstrap(const std::string &os,
                    std::vector<Peripheral *> peripherals) {
  _halt = false;
  (void)os; // load program
  auto idx = 0;
  for (auto peripheral : peripherals) {
    peripheral->bootstrap(nullptr, dynamic_cast<InterruptHandler *>(this));
    idx++;
  }
}

bool CPU::halt() const { return _halt; }

void CPU::power_off() { _halt = true; }

void CPU::execute() { _execution_unit.fetch_decode_execute(); }

void CPU::interrupt(uint8_t type) { _execution_unit.interrupt(type); }
