#include "instruction_set.h"

#include "in.h"
#include "lahf.h"
#include "lds.h"
#include "lea.h"
#include "les.h"
#include "mov.h"
#include "noop.h"
#include "out.h"
#include "pop.h"
#include "push.h"
#include "sahf.h"
#include "xchg.h"
#include "xlat.h"

#include <algorithm>

#define _INSTRUCTION(opcode, mask, cls)                                        \
  register_instruction<cls>(MicroOp::Key(opcode, mask))

InstructionSet::InstructionSet() {
  _INSTRUCTION(0x88, 0xFC, MovRegisterAndMemory);
  _INSTRUCTION(0xB0, 0xF0, MovRegisterImmediate);
  _INSTRUCTION(0xA0, 0xF0, MovAccumulator);
  _INSTRUCTION(0x8E, 0xFE, MovSegmentAndRegisterMemory);
  _INSTRUCTION(0xC6, 0xFE, MovMemoryImmediate);
  _INSTRUCTION(0x50, 0xF0, PushRegister);
  _INSTRUCTION(0xFF, 0xFF, PushMemory);
  _INSTRUCTION(0x06, 0xE7, PushSegment);
  _INSTRUCTION(0x9C, 0xFF, PushFlags);
  _INSTRUCTION(0x58, 0xF8, PopRegister);
  _INSTRUCTION(0x8F, 0xFF, PopMemory);
  _INSTRUCTION(0x07, 0xE7, PopSegment);
  _INSTRUCTION(0x9D, 0xFF, PopFlags);
  _INSTRUCTION(0x90, 0xF8, XCHGAccumulator);
  _INSTRUCTION(0x86, 0xFE, XCHGRegisterMemory);
  _INSTRUCTION(0xE4, 0xFE, INPort);
  _INSTRUCTION(0xEC, 0xFE, INDX);
  _INSTRUCTION(0xE6, 0xFE, OUTPort);
  _INSTRUCTION(0xEE, 0xFE, OUTDX);
  _INSTRUCTION(0xD7, 0xD6, XLAT);
  _INSTRUCTION(0x8D, 0xFF, LEA);
  _INSTRUCTION(0xC4, 0xFF, LES);
  _INSTRUCTION(0xC5, 0xFF, LDS);
  _INSTRUCTION(0x9F, 0xFF, LAHF);
  _INSTRUCTION(0x9E, 0xFF, SAHF);
  _INSTRUCTION(0x90, 0xFF, NOP);
  //_INSTRUCTION(0xff, 0xFF, AAD);
  //_INSTRUCTION(0xff, 0xFF, OR);
  //_INSTRUCTION(0xff, 0xFF, ADC);
  //_INSTRUCTION(0xff, 0xFF, SBB);
  //_INSTRUCTION(0xff, 0xFF, ADD); // TODO
  //_INSTRUCTION(0xff, 0xFF, DAA);
  //_INSTRUCTION(0xff, 0xFF, SUB); // TODO
  //_INSTRUCTION(0xff, 0xFF, DAS);
  //_INSTRUCTION(0xff, 0xFF, XOR); // TODO
  //_INSTRUCTION(0xff, 0xFF, AAA);
  //_INSTRUCTION(0xff, 0xFF, AAS);
  //_INSTRUCTION(0xff, 0xFF, TEST);
  //_INSTRUCTION(0xff, 0xFF, CBW);
  //_INSTRUCTION(0xff, 0xFF, CWD);
  //_INSTRUCTION(0xff, 0xFF, WAIT);  // TODO
  //_INSTRUCTION(0xff, 0xFF, MOVS);
  //_INSTRUCTION(0xff, 0xFF, CMPS);
  //_INSTRUCTION(0xff, 0xFF, STOS);
  //_INSTRUCTION(0xff, 0xFF, LODS);
  //_INSTRUCTION(0xff, 0xFF, SCAS);
  //_INSTRUCTION(0xff, 0xFF, RET);
  //_INSTRUCTION(0xff, 0xFF, RETF);
  //_INSTRUCTION(0xff, 0xFF, INT);
  //_INSTRUCTION(0xff, 0xFF, IRET);
  //_INSTRUCTION(0xff, 0xFF, AAM);
  //_INSTRUCTION(0xff, 0xFF, AAD);
  //_INSTRUCTION(0xff, 0xFF, LOCK);
  //_INSTRUCTION(0xff, 0xFF, HALT); // TODO
  //_INSTRUCTION(0xff, 0xFF, CMC);
  //_INSTRUCTION(0xff, 0xFF, CLC);
  //_INSTRUCTION(0xff, 0xFF, STC);
  //_INSTRUCTION(0xff, 0xFF, CLI); // TODO
  //_INSTRUCTION(0xff, 0xFF, STI);
  //_INSTRUCTION(0xff, 0xFF, CLD);
  //_INSTRUCTION(0xff, 0xFF, STD);
  //_INSTRUCTION(0xff, 0xFF, SHL); // TODO
  //_INSTRUCTION(0xff, 0xFF, SHR);

  /**
   * Code defination
   * E - Argument(s) encoded in ModR/M
   * I - Immediate value
   *
   * Argument Size
   * b - byte
   * w - word
   */

  _instruction_code_map.insert(
      {0x00, InstructionCode("ADD", "E", "E")}); // opcode mode [disp] [data]
  _instruction_code_map.insert({0x01, InstructionCode("ADD", "E", "E")});
  _instruction_code_map.insert({0x02, InstructionCode("ADD", "E", "E")});
  _instruction_code_map.insert({0x03, InstructionCode("ADD", "E", "E")});
  _instruction_code_map.insert({0x04, InstructionCode("ADD", "E", "Ib")});
  _instruction_code_map.insert({0x05, InstructionCode("ADD", "E", "Iw")});
  _instruction_code_map.insert({0x06, InstructionCode("PUSH")}); // opcode
  _instruction_code_map.insert({0x07, InstructionCode("POP")});  // opcode
  _instruction_code_map.insert(
      {0x08, InstructionCode("OR", "E", "E")}); // opcode mode [disp] [data]
  _instruction_code_map.insert({0x09, InstructionCode("OR", "E", "E")});
  _instruction_code_map.insert({0x0A, InstructionCode("OR", "E", "E")});
  _instruction_code_map.insert({0x0B, InstructionCode("OR", "E", "E")});
  _instruction_code_map.insert({0x0C, InstructionCode("OR", "E", "Ib")});
  _instruction_code_map.insert({0x0D, InstructionCode("OR", "E", "Iw")});
  _instruction_code_map.insert({0x0E, InstructionCode("PUSH")}); // opcode
  _instruction_code_map.insert({0x0F, InstructionCode()});
  _instruction_code_map.insert(
      {0x10, InstructionCode("ADC", "E", "E")}); // opcode mode [disp] [data]
  _instruction_code_map.insert({0x11, InstructionCode("ADC", "E", "E")});
  _instruction_code_map.insert({0x12, InstructionCode("ADC", "E", "E")});
  _instruction_code_map.insert({0x13, InstructionCode("ADC", "E", "E")});
  _instruction_code_map.insert({0x14, InstructionCode("ADC", "E", "Ib")});
  _instruction_code_map.insert({0x15, InstructionCode("ADC", "E", "Iw")});
  _instruction_code_map.insert({0x16, InstructionCode("PUSH")}); // opcode
  _instruction_code_map.insert({0x17, InstructionCode("POP")});  // opcode
  _instruction_code_map.insert(
      {0x18, InstructionCode("SBB", "E", "E")}); // opcode mode [disp] [data]
  _instruction_code_map.insert({0x19, InstructionCode("SBB", "E", "E")});
  _instruction_code_map.insert({0x1A, InstructionCode("SBB", "E", "E")});
  _instruction_code_map.insert({0x1B, InstructionCode("SBB", "E", "E")});
  _instruction_code_map.insert({0x1C, InstructionCode("SBB", "E", "Ib")});
  _instruction_code_map.insert({0x1D, InstructionCode("SBB", "E", "Iw")});
  _instruction_code_map.insert({0x1E, InstructionCode("PUSH")}); // opcode
  _instruction_code_map.insert({0x1F, InstructionCode("POP")});  // opcode
  _instruction_code_map.insert(
      {0x20, InstructionCode("AND", "E", "E")}); // opcode mode [disp] [data]
  _instruction_code_map.insert({0x21, InstructionCode("AND", "E", "E")});
  _instruction_code_map.insert({0x22, InstructionCode("AND", "E", "E")});
  _instruction_code_map.insert({0x23, InstructionCode("AND", "E", "E")});
  _instruction_code_map.insert({0x24, InstructionCode("AND", "E", "Ib")});
  _instruction_code_map.insert({0x25, InstructionCode("AND", "E", "Iw")});
  _instruction_code_map.insert({0x26, InstructionCode("ES")});  // ?
  _instruction_code_map.insert({0x27, InstructionCode("DAA")}); // opcode
  _instruction_code_map.insert(
      {0x28, InstructionCode("SUB", "E", "E")}); // opcode mode [disp] [data]
  _instruction_code_map.insert({0x29, InstructionCode("SUB", "E", "E")});
  _instruction_code_map.insert({0x2A, InstructionCode("SUB", "E", "E")});
  _instruction_code_map.insert({0x2B, InstructionCode("SUB", "E", "E")});
  _instruction_code_map.insert({0x2C, InstructionCode("SUB", "E", "Ib")});
  _instruction_code_map.insert({0x2D, InstructionCode("SUB", "E", "Iw")});
  _instruction_code_map.insert({0x2E, InstructionCode("CS")});  // ?
  _instruction_code_map.insert({0x2F, InstructionCode("DAS")}); // opcode
  _instruction_code_map.insert(
      {0x30, InstructionCode("XOR", "E", "E")}); // opcode mode [disp] [data]
  _instruction_code_map.insert({0x31, InstructionCode("XOR", "E", "E")});
  _instruction_code_map.insert({0x32, InstructionCode("XOR", "E", "E")});
  _instruction_code_map.insert({0x33, InstructionCode("XOR", "E", "E")});
  _instruction_code_map.insert({0x34, InstructionCode("XOR", "E", "Ib")});
  _instruction_code_map.insert({0x35, InstructionCode("XOR", "E", "Iw")});
  _instruction_code_map.insert({0x36, InstructionCode("SS")});  // ?
  _instruction_code_map.insert({0x37, InstructionCode("AAA")}); // opcode
  _instruction_code_map.insert(
      {0x38, InstructionCode("CMP", "E", "E")}); // opcode mode [disp] [data]
  _instruction_code_map.insert({0x39, InstructionCode("CMP", "E", "E")});
  _instruction_code_map.insert({0x3A, InstructionCode("CMP", "E", "E")});
  _instruction_code_map.insert({0x3B, InstructionCode("CMP", "E", "E")});
  _instruction_code_map.insert({0x3C, InstructionCode("CMP", "E", "Ib")});
  _instruction_code_map.insert({0x3D, InstructionCode("CMP", "E", "Iw")});
  _instruction_code_map.insert({0x3E, InstructionCode("DS")});  // ?
  _instruction_code_map.insert({0x3F, InstructionCode("AAS")}); // opcode
  _instruction_code_map.insert(
      {0x40, InstructionCode("INC", "E")}); // opcode mode [disp]
  _instruction_code_map.insert({0x41, InstructionCode("INC", "E")});
  _instruction_code_map.insert({0x42, InstructionCode("INC", "E")});
  _instruction_code_map.insert({0x43, InstructionCode("INC", "E")});
  _instruction_code_map.insert({0x44, InstructionCode("INC", "E")});
  _instruction_code_map.insert({0x45, InstructionCode("INC", "E")});
  _instruction_code_map.insert({0x46, InstructionCode("INC", "E")});
  _instruction_code_map.insert({0x47, InstructionCode("INC", "E")});
  _instruction_code_map.insert(
      {0x48, InstructionCode("DEC", "E")}); // opcode mode [disp]
  _instruction_code_map.insert({0x49, InstructionCode("DEC", "E")});
  _instruction_code_map.insert({0x4A, InstructionCode("DEC", "E")});
  _instruction_code_map.insert({0x4B, InstructionCode("DEC", "E")});
  _instruction_code_map.insert({0x4C, InstructionCode("DEC", "E")});
  _instruction_code_map.insert({0x4D, InstructionCode("DEC", "E")});
  _instruction_code_map.insert({0x4E, InstructionCode("DEC", "E")});
  _instruction_code_map.insert({0x4F, InstructionCode("DEC", "E")});
  _instruction_code_map.insert({0x50, InstructionCode("PUSH")}); // opcode
  _instruction_code_map.insert({0x51, InstructionCode("PUSH")});
  _instruction_code_map.insert({0x52, InstructionCode("PUSH")});
  _instruction_code_map.insert({0x53, InstructionCode("PUSH")});
  _instruction_code_map.insert({0x54, InstructionCode("PUSH")});
  _instruction_code_map.insert({0x55, InstructionCode("PUSH")});
  _instruction_code_map.insert({0x56, InstructionCode("PUSH")});
  _instruction_code_map.insert({0x57, InstructionCode("PUSH")});
  _instruction_code_map.insert({0x58, InstructionCode("POP")});
  _instruction_code_map.insert({0x59, InstructionCode("POP")});
  _instruction_code_map.insert({0x5A, InstructionCode("POP")});
  _instruction_code_map.insert({0x5B, InstructionCode("POP")});
  _instruction_code_map.insert({0x5C, InstructionCode("POP")});
  _instruction_code_map.insert({0x5D, InstructionCode("POP")});
  _instruction_code_map.insert({0x5E, InstructionCode("POP")});
  _instruction_code_map.insert({0x5F, InstructionCode("POP")});
  _instruction_code_map.insert({0x60, InstructionCode()});
  _instruction_code_map.insert({0x61, InstructionCode()});
  _instruction_code_map.insert({0x62, InstructionCode()});
  _instruction_code_map.insert({0x63, InstructionCode()});
  _instruction_code_map.insert({0x64, InstructionCode()});
  _instruction_code_map.insert({0x65, InstructionCode()});
  _instruction_code_map.insert({0x66, InstructionCode()});
  _instruction_code_map.insert({0x67, InstructionCode()});
  _instruction_code_map.insert({0x68, InstructionCode()});
  _instruction_code_map.insert({0x69, InstructionCode()});
  _instruction_code_map.insert({0x6A, InstructionCode()});
  _instruction_code_map.insert({0x6B, InstructionCode()});
  _instruction_code_map.insert({0x6C, InstructionCode()});
  _instruction_code_map.insert({0x6D, InstructionCode()});
  _instruction_code_map.insert({0x6E, InstructionCode()});
  _instruction_code_map.insert({0x6F, InstructionCode()});
  _instruction_code_map.insert(
      {0x70, InstructionCode("JO", "Ib")}); // opcode disp
  _instruction_code_map.insert({0x71, InstructionCode("JNO", "Ib")});
  _instruction_code_map.insert({0x72, InstructionCode("JB", "Ib")});
  _instruction_code_map.insert({0x73, InstructionCode("JNB", "Ib")});
  _instruction_code_map.insert({0x74, InstructionCode("JZ", "Ib")});
  _instruction_code_map.insert({0x75, InstructionCode("JNZ", "Ib")});
  _instruction_code_map.insert({0x76, InstructionCode("JBE", "Ib")});
  _instruction_code_map.insert({0x77, InstructionCode("JA", "Ib")});
  _instruction_code_map.insert({0x78, InstructionCode("JS", "Ib")});
  _instruction_code_map.insert({0x79, InstructionCode("JNS", "Ib")});
  _instruction_code_map.insert({0x7A, InstructionCode("JPE", "Ib")});
  _instruction_code_map.insert({0x7B, InstructionCode("JPO", "Ib")});
  _instruction_code_map.insert({0x7C, InstructionCode("JL", "Ib")});
  _instruction_code_map.insert({0x7D, InstructionCode("JGE", "Ib")});
  _instruction_code_map.insert({0x7E, InstructionCode("JLE", "Ib")});
  _instruction_code_map.insert({0x7F, InstructionCode("JG", "Ib")});
  _instruction_code_map.insert(
      {0x80, InstructionCode("GRP1", "E", "Ib")}); // opcode mode [disp] [data]
  _instruction_code_map.insert({0x81, InstructionCode("GRP1", "E", "Iw")});
  _instruction_code_map.insert({0x82, InstructionCode("GRP1", "E", "Ib")});
  _instruction_code_map.insert({0x83, InstructionCode("GRP1", "E", "Ib")});
  _instruction_code_map.insert({0x84, InstructionCode("TEST", "E", "E")});
  _instruction_code_map.insert({0x85, InstructionCode("TEST", "E", "E")});
  _instruction_code_map.insert({0x86, InstructionCode("XCHG", "E", "E")});
  _instruction_code_map.insert({0x87, InstructionCode("XCHG", "E", "E")});
  _instruction_code_map.insert(
      {0x88, InstructionCode("MOV", "E", "E")}); // opcode mode [disp] -- v
  _instruction_code_map.insert({0x89, InstructionCode("MOV", "E", "E")});
  _instruction_code_map.insert({0x8A, InstructionCode("MOV", "E", "E")});
  _instruction_code_map.insert({0x8B, InstructionCode("MOV", "E", "E")});
  _instruction_code_map.insert({0x8C, InstructionCode("MOV", "E", "E")});
  _instruction_code_map.insert(
      {0x8D, InstructionCode("LEA", "E")}); // opcode mode [disp]
  _instruction_code_map.insert(
      {0x8E, InstructionCode("MOV", "E", "E")}); // opcode mode [disp] -- v
  _instruction_code_map.insert(
      {0x8F, InstructionCode("POP", "E")});                     // opcode mode
  _instruction_code_map.insert({0x90, InstructionCode("NOP")}); // opcode
  _instruction_code_map.insert(
      {0x91, InstructionCode("XCHG", "E")}); // opcode mode
  _instruction_code_map.insert({0x92, InstructionCode("XCHG", "E")});
  _instruction_code_map.insert({0x93, InstructionCode("XCHG", "E")});
  _instruction_code_map.insert({0x94, InstructionCode("XCHG", "E")});
  _instruction_code_map.insert({0x95, InstructionCode("XCHG", "E")});
  _instruction_code_map.insert({0x96, InstructionCode("XCHG", "E")});
  _instruction_code_map.insert({0x97, InstructionCode("XCHG", "E")});
  _instruction_code_map.insert({0x98, InstructionCode("CBW")}); // opcode
  _instruction_code_map.insert({0x99, InstructionCode("CWD")}); // opcode
  _instruction_code_map.insert(
      {0x9A, InstructionCode("CALL", "E")});                      // opcode mode
  _instruction_code_map.insert({0x9B, InstructionCode("WAIT")});  // opcode
  _instruction_code_map.insert({0x9C, InstructionCode("PUSHF")}); // opcode
  _instruction_code_map.insert({0x9D, InstructionCode("POPF")});
  _instruction_code_map.insert({0x9E, InstructionCode("SAHF")});
  _instruction_code_map.insert({0x9F, InstructionCode("LAHF")});
  _instruction_code_map.insert({0xA0, InstructionCode("MOV")}); // opcode -- v
  _instruction_code_map.insert({0xA1, InstructionCode("MOV")});
  _instruction_code_map.insert({0xA2, InstructionCode("MOV")});
  _instruction_code_map.insert({0xA3, InstructionCode("MOV")});
  _instruction_code_map.insert({0xA4, InstructionCode("MOVSB")}); // opcode
  _instruction_code_map.insert({0xA5, InstructionCode("MOVSW")});
  _instruction_code_map.insert({0xA6, InstructionCode("CMPSB")});
  _instruction_code_map.insert({0xA7, InstructionCode("CMPSW")});
  _instruction_code_map.insert(
      {0xA8, InstructionCode("TEST", "E", "Ib")}); // opcode mode
  _instruction_code_map.insert({0xA9, InstructionCode("TEST", "E", "Iw")});
  _instruction_code_map.insert({0xAA, InstructionCode("STOSB")}); // opcode
  _instruction_code_map.insert({0xAB, InstructionCode("STOSW")});
  _instruction_code_map.insert({0xAC, InstructionCode("LODSB")});
  _instruction_code_map.insert({0xAD, InstructionCode("LODSW")});
  _instruction_code_map.insert({0xAE, InstructionCode("SCASB")});
  _instruction_code_map.insert({0xAF, InstructionCode("SCASW")});
  _instruction_code_map.insert(
      {0xB0, InstructionCode("MOV", "Ib")}); // opcode data -- v
  _instruction_code_map.insert({0xB1, InstructionCode("MOV", "Ib")});
  _instruction_code_map.insert({0xB2, InstructionCode("MOV", "Ib")});
  _instruction_code_map.insert({0xB3, InstructionCode("MOV", "Ib")});
  _instruction_code_map.insert({0xB4, InstructionCode("MOV", "Ib")});
  _instruction_code_map.insert({0xB5, InstructionCode("MOV", "Ib")});
  _instruction_code_map.insert({0xB6, InstructionCode("MOV", "Ib")});
  _instruction_code_map.insert({0xB7, InstructionCode("MOV", "Ib")});
  _instruction_code_map.insert({0xB8, InstructionCode("MOV", "Iw")});
  _instruction_code_map.insert({0xB9, InstructionCode("MOV", "Iw")});
  _instruction_code_map.insert({0xBA, InstructionCode("MOV", "Iw")});
  _instruction_code_map.insert({0xBB, InstructionCode("MOV", "Iw")});
  _instruction_code_map.insert({0xBC, InstructionCode("MOV", "Iw")});
  _instruction_code_map.insert({0xBD, InstructionCode("MOV", "Iw")});
  _instruction_code_map.insert({0xBE, InstructionCode("MOV", "Iw")});
  _instruction_code_map.insert({0xBF, InstructionCode("MOV", "Iw")});
  _instruction_code_map.insert({0xC0, InstructionCode()});
  _instruction_code_map.insert({0xC1, InstructionCode()});
  _instruction_code_map.insert(
      {0xC2, InstructionCode("RET", "Iw")});                    // opcode data
  _instruction_code_map.insert({0xC3, InstructionCode("RET")}); // opcode
  _instruction_code_map.insert(
      {0xC4, InstructionCode("LES", "E", "E")}); // opcode mode
  _instruction_code_map.insert({0xC5, InstructionCode("LDS", "E", "E")});
  _instruction_code_map.insert(
      {0xC6, InstructionCode("MOV", "E", "Ib")}); // opcode mode [disp] data
  _instruction_code_map.insert({0xC7, InstructionCode("MOV", "E", "Iw")});
  _instruction_code_map.insert({0xC8, InstructionCode()});
  _instruction_code_map.insert({0xC9, InstructionCode()});
  _instruction_code_map.insert(
      {0xCA, InstructionCode("RETF", "Iw")});                    // opcode data
  _instruction_code_map.insert({0xCB, InstructionCode("RETF")}); // opcode
  _instruction_code_map.insert(
      {0xCC, InstructionCode("INT", "3")}); // opcode data
  _instruction_code_map.insert({0xCD, InstructionCode("INT", "Ib")});
  _instruction_code_map.insert({0xCE, InstructionCode("INTO")}); // opcode
  _instruction_code_map.insert({0xCF, InstructionCode("IRET")}); // opcode
  _instruction_code_map.insert(
      {0xD0, InstructionCode("GRP2", "E", "1")}); // opcode data
  _instruction_code_map.insert({0xD1, InstructionCode("GRP2", "E", "1")});
  _instruction_code_map.insert(
      {0xD2, InstructionCode("GRP2", "E", "E")}); // opcode mode
  _instruction_code_map.insert({0xD3, InstructionCode("GRP2", "E", "E")});
  _instruction_code_map.insert({0xD4, InstructionCode("AAM", "I0")}); // ?
  _instruction_code_map.insert({0xD5, InstructionCode("AAD", "I0")});
  _instruction_code_map.insert({0xD6, InstructionCode()});
  _instruction_code_map.insert({0xD7, InstructionCode("XLAT")}); // opcode
  _instruction_code_map.insert({0xD8, InstructionCode()});
  _instruction_code_map.insert({0xD9, InstructionCode()});
  _instruction_code_map.insert({0xDA, InstructionCode()});
  _instruction_code_map.insert({0xDB, InstructionCode()});
  _instruction_code_map.insert({0xDC, InstructionCode()});
  _instruction_code_map.insert({0xDD, InstructionCode()});
  _instruction_code_map.insert({0xDE, InstructionCode()});
  _instruction_code_map.insert({0xDF, InstructionCode()});
  _instruction_code_map.insert(
      {0xE0, InstructionCode("LOOPNZ", "Ib")}); // opcode data
  _instruction_code_map.insert({0xE1, InstructionCode("LOOPZ", "Ib")});
  _instruction_code_map.insert({0xE2, InstructionCode("LOOP", "Ib")});
  _instruction_code_map.insert({0xE3, InstructionCode("JCXZ", "Ib")});
  _instruction_code_map.insert(
      {0xE4, InstructionCode("IN", "E", "Ib")}); // opcode mode data
  _instruction_code_map.insert({0xE5, InstructionCode("IN", "E", "Ib")});
  _instruction_code_map.insert(
      {0xE6, InstructionCode("OUT", "Ib", "E")}); // opcode mode data
  _instruction_code_map.insert({0xE7, InstructionCode("OUT", "Ib", "E")});
  _instruction_code_map.insert(
      {0xE8, InstructionCode("CALL", "Iv")}); // opcode data
  _instruction_code_map.insert(
      {0xE9, InstructionCode("JMP", "Iv")}); // opcode data
  _instruction_code_map.insert({0xEA, InstructionCode("JMP", "Iv")});
  _instruction_code_map.insert({0xEB, InstructionCode("JMP", "Ib")});
  _instruction_code_map.insert(
      {0xEC, InstructionCode("IN", "E", "E")}); // opcode mode
  _instruction_code_map.insert({0xED, InstructionCode("IN", "E", "E")});
  _instruction_code_map.insert({0xEE, InstructionCode("OUT", "E", "E")});
  _instruction_code_map.insert({0xEF, InstructionCode("OUT", "E", "E")});
  _instruction_code_map.insert({0xF0, InstructionCode("LOCK")}); // opcode
  _instruction_code_map.insert({0xF1, InstructionCode()});
  _instruction_code_map.insert({0xF2, InstructionCode("REPNZ")});
  _instruction_code_map.insert({0xF3, InstructionCode("REPZ")});
  _instruction_code_map.insert({0xF4, InstructionCode("HLT")});
  _instruction_code_map.insert({0xF5, InstructionCode("CMC")});
  _instruction_code_map.insert({0xF6, InstructionCode("GRP3a", "E")});
  _instruction_code_map.insert({0xF7, InstructionCode("GRP3b", "E")});
  _instruction_code_map.insert({0xF8, InstructionCode("CLC")});
  _instruction_code_map.insert({0xF9, InstructionCode("STC")});
  _instruction_code_map.insert({0xFA, InstructionCode("CLI")});
  _instruction_code_map.insert({0xFB, InstructionCode("STI")});
  _instruction_code_map.insert({0xFC, InstructionCode("CLD")});
  _instruction_code_map.insert({0xFD, InstructionCode("STD")});
  _instruction_code_map.insert({0xFE, InstructionCode("GRP4", "E")});
  _instruction_code_map.insert({0xFF, InstructionCode("GRP5", "E")});
}

InstructionSet::create_func_t InstructionSet::find(const MicroOp::Key &key) {
  auto it =
      std::find_if(instructions.begin(), instructions.end(),
                   [&key](const std::pair<MicroOp::Key, create_func_t> &rhs) {
                     return rhs.first == key;
                   });
  if (it == std::end(instructions)) {
    return (create_func_t) nullptr;
  }
  return it->second;
}

InstructionCode *InstructionSet::find(uint8_t opcode) {
  auto it = _instruction_code_map.find(opcode);
  if (it == _instruction_code_map.end())
    return nullptr;
  return &(it->second);
}

std::shared_ptr<MicroOp> InstructionSet::decode(uint8_t opcode,
                                                const MicroOp::Params &params) {
  auto _ctor = find(MicroOp::Key(opcode));
  if (_ctor == nullptr) {
    assert(false);
    return std::shared_ptr<MicroOp>();
  }
  return _ctor(params);
}
