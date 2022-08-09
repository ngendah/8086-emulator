/*
 * CPU.cpp
 *
 */

#include "cpu.h"

CPU::CPU(BUS *ram, Decoder *decoder) : _ram(ram), _decoder(decoder) {}

CPU::~CPU() {}

void CPU::start() {}
