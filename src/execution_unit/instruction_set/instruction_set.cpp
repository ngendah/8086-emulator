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

// reference: http://www.mlsite.net/8086

#define _INSTRUCTION(opcode, mask, cls)                               \ register_instruction<cls>(MicroOp::Key(opcode, mask))

InstructionSet::InstructionSet() {
  _INSTRUCTION(0x88, 0xFC, MovRegisterAndMemory);
  _INSTRUCTION(0xB0, 0xF0, MovRegisterImmediate);
  _INSTRUCTION(0xA0, 0xF0, MovAccumulator);
  _INSTRUCTION(0x8E, 0xFC, MovSegmentAndRegisterMemory);
  _INSTRUCTION(0xC6, 0xFE, MovMemoryImmediate);
  _INSTRUCTION(0x50, 0xF0, PushRegister);
  _INSTRUCTION(0xFF, 0xFF, PushMemory);
  _INSTRUCTION(0x06, 0xE7, PushSegment);
  // _INSTRUCTION(0x9C, 0xFF, PushFlags);
  _INSTRUCTION(0x58, 0xF8, PopRegister);
  _INSTRUCTION(0x8F, 0xFF, PopMemory);
  _INSTRUCTION(0x07, 0xE7, PopSegment);
  // _INSTRUCTION(0x9D, 0xFF, PopFlags);
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

  /**
   * Code defination
   * E - Argument(s) encoded in Mode 
   * I - Immediate value
   *
   * Argument Size
   * b - byte
   * w - word
   */

_instruction_code_map.insert({0x00, InstructionCode(0x00, "ADD", "E", "E")});   // opcode mode [disp] [data]
_instruction_code_map.insert({0x01, InstructionCode(0x01, "ADD", "E", "E")});
_instruction_code_map.insert({0x02, InstructionCode(0x02, "ADD", "E", "E")});
_instruction_code_map.insert({0x03, InstructionCode(0x03, "ADD", "E", "E")});
_instruction_code_map.insert({0x04, InstructionCode(0x04, "ADD", "E", "Ib")});
_instruction_code_map.insert({0x05, InstructionCode(0x05, "ADD", "E", "Iw")});
_instruction_code_map.insert({0x06, InstructionCode(0x06, "PUSH")});            // opcode 
_instruction_code_map.insert({0x07, InstructionCode(0x07, "POP")});             // opcode
_instruction_code_map.insert({0x08, InstructionCode(0x08, "OR", "E", "E")});    // opcode mode [disp] [data] 
_instruction_code_map.insert({0x09, InstructionCode(0x09, "OR", "E", "E")});
_instruction_code_map.insert({0x0A, InstructionCode(0x0A, "OR", "E", "E")});
_instruction_code_map.insert({0x0B, InstructionCode(0x0B, "OR", "E", "E")});
_instruction_code_map.insert({0x0C, InstructionCode(0x0C, "OR", "E", "Ib")});
_instruction_code_map.insert({0x0D, InstructionCode(0x0D, "OR", "E", "Iw")});
_instruction_code_map.insert({0x0E, InstructionCode(0x0E, "PUSH")});            // opcode
_instruction_code_map.insert({0x0F, InstructionCode(0x0F)});
_instruction_code_map.insert({0x10, InstructionCode(0x10, "ADC", "E", "E")});   // opcode mode [disp] [data]
_instruction_code_map.insert({0x11, InstructionCode(0x11, "ADC", "E", "E")});
_instruction_code_map.insert({0x12, InstructionCode(0x12, "ADC", "E", "E")});
_instruction_code_map.insert({0x13, InstructionCode(0x13, "ADC", "E", "E")});
_instruction_code_map.insert({0x14, InstructionCode(0x14, "ADC", "E", "Ib")});
_instruction_code_map.insert({0x15, InstructionCode(0x15, "ADC", "E", "Iw")});
_instruction_code_map.insert({0x16, InstructionCode(0x16, "PUSH")});            // opcode
_instruction_code_map.insert({0x17, InstructionCode(0x17, "POP")});             // opcode
_instruction_code_map.insert({0x18, InstructionCode(0x18, "SBB", "E", "E")});   // opcode mode [disp] [data]
_instruction_code_map.insert({0x19, InstructionCode(0x19, "SBB", "E", "E")});
_instruction_code_map.insert({0x1A, InstructionCode(0x1A, "SBB", "E", "E")});
_instruction_code_map.insert({0x1B, InstructionCode(0x1B, "SBB", "E", "E")});
_instruction_code_map.insert({0x1C, InstructionCode(0x1C, "SBB", "E", "Ib")});
_instruction_code_map.insert({0x1D, InstructionCode(0x1D, "SBB", "E", "Iw")});
_instruction_code_map.insert({0x1E, InstructionCode(0x1E, "PUSH")});            // opcode
_instruction_code_map.insert({0x1F, InstructionCode(0x1F, "POP")});             // opcode
_instruction_code_map.insert({0x20, InstructionCode(0x20, "AND", "E", "E")});   // opcode mode [disp] [data]
_instruction_code_map.insert({0x21, InstructionCode(0x21, "AND", "E", "E")});
_instruction_code_map.insert({0x22, InstructionCode(0x22, "AND", "E", "E")});
_instruction_code_map.insert({0x23, InstructionCode(0x23, "AND", "E", "E")});
_instruction_code_map.insert({0x24, InstructionCode(0x24, "AND", "E", "Ib")});
_instruction_code_map.insert({0x25, InstructionCode(0x25, "AND", "E", "Iw")});
_instruction_code_map.insert({0x26, InstructionCode(0x26, "ES")});              // ?
_instruction_code_map.insert({0x27, InstructionCode(0x27, "DAA")});             // opcode
_instruction_code_map.insert({0x28, InstructionCode(0x28, "SUB", "E", "E")});   // opcode mode [disp] [data]
_instruction_code_map.insert({0x29, InstructionCode(0x29, "SUB", "E", "E")});
_instruction_code_map.insert({0x2A, InstructionCode(0x2A, "SUB", "E", "E")});
_instruction_code_map.insert({0x2B, InstructionCode(0x2B, "SUB", "E", "E")});
_instruction_code_map.insert({0x2C, InstructionCode(0x2C, "SUB", "E", "Ib")});
_instruction_code_map.insert({0x2D, InstructionCode(0x2D, "SUB", "E", "Iw")});
_instruction_code_map.insert({0x2E, InstructionCode(0x2E, "CS")});              // ?
_instruction_code_map.insert({0x2F, InstructionCode(0x2F, "DAS")});             // opcode
_instruction_code_map.insert({0x30, InstructionCode(0x30, "XOR", "E", "E")});   // opcode mode [disp] [data]
_instruction_code_map.insert({0x31, InstructionCode(0x31, "XOR", "E", "E")});
_instruction_code_map.insert({0x32, InstructionCode(0x32, "XOR", "E", "E")});
_instruction_code_map.insert({0x33, InstructionCode(0x33, "XOR", "E", "E")});
_instruction_code_map.insert({0x34, InstructionCode(0x34, "XOR", "E", "Ib")});
_instruction_code_map.insert({0x35, InstructionCode(0x35, "XOR", "E", "Iw")});
_instruction_code_map.insert({0x36, InstructionCode(0x36, "SS")});              // ?
_instruction_code_map.insert({0x37, InstructionCode(0x37, "AAA")});             // opcode
_instruction_code_map.insert({0x38, InstructionCode(0x38, "CMP", "E", "E")});   // opcode mode [disp] [data]
_instruction_code_map.insert({0x39, InstructionCode(0x39, "CMP", "E", "E")});
_instruction_code_map.insert({0x3A, InstructionCode(0x3A, "CMP", "E", "E")});
_instruction_code_map.insert({0x3B, InstructionCode(0x3B, "CMP", "E", "E")});
_instruction_code_map.insert({0x3C, InstructionCode(0x3C, "CMP", "E", "Ib")});
_instruction_code_map.insert({0x3D, InstructionCode(0x3D, "CMP", "E", "Iw")});
_instruction_code_map.insert({0x3E, InstructionCode(0x3E, "DS")});              // ?
_instruction_code_map.insert({0x3F, InstructionCode(0x3F, "AAS")});             // opcode
_instruction_code_map.insert({0x40, InstructionCode(0x40, "INC", "E")});        // opcode mode [disp]
_instruction_code_map.insert({0x41, InstructionCode(0x41, "INC", "E")});
_instruction_code_map.insert({0x42, InstructionCode(0x42, "INC", "E")});
_instruction_code_map.insert({0x43, InstructionCode(0x43, "INC", "E")});
_instruction_code_map.insert({0x44, InstructionCode(0x44, "INC", "E")});
_instruction_code_map.insert({0x45, InstructionCode(0x45, "INC", "E")});
_instruction_code_map.insert({0x46, InstructionCode(0x46, "INC", "E")});
_instruction_code_map.insert({0x47, InstructionCode(0x47, "INC", "E")});
_instruction_code_map.insert({0x48, InstructionCode(0x48, "DEC", "E")});        // opcode mode [disp]
_instruction_code_map.insert({0x49, InstructionCode(0x49, "DEC", "E")});
_instruction_code_map.insert({0x4A, InstructionCode(0x4A, "DEC", "E")});
_instruction_code_map.insert({0x4B, InstructionCode(0x4B, "DEC", "E")});
_instruction_code_map.insert({0x4C, InstructionCode(0x4C, "DEC", "E")});
_instruction_code_map.insert({0x4D, InstructionCode(0x4D, "DEC", "E")});
_instruction_code_map.insert({0x4E, InstructionCode(0x4E, "DEC", "E")});
_instruction_code_map.insert({0x4F, InstructionCode(0x4F, "DEC", "E")});
_instruction_code_map.insert({0x50, InstructionCode(0x50, "PUSH")});            // opcode
_instruction_code_map.insert({0x51, InstructionCode(0x51, "PUSH")});
_instruction_code_map.insert({0x52, InstructionCode(0x52, "PUSH")});
_instruction_code_map.insert({0x53, InstructionCode(0x53, "PUSH")});
_instruction_code_map.insert({0x54, InstructionCode(0x54, "PUSH")});
_instruction_code_map.insert({0x55, InstructionCode(0x55, "PUSH")});
_instruction_code_map.insert({0x56, InstructionCode(0x56, "PUSH")});
_instruction_code_map.insert({0x57, InstructionCode(0x57, "PUSH")});
_instruction_code_map.insert({0x58, InstructionCode(0x58, "POP")});
_instruction_code_map.insert({0x59, InstructionCode(0x59, "POP")});
_instruction_code_map.insert({0x5A, InstructionCode(0x5A, "POP")});
_instruction_code_map.insert({0x5B, InstructionCode(0x5B, "POP")});
_instruction_code_map.insert({0x5C, InstructionCode(0x5C, "POP")});
_instruction_code_map.insert({0x5D, InstructionCode(0x5D, "POP")});
_instruction_code_map.insert({0x5E, InstructionCode(0x5E, "POP")});
_instruction_code_map.insert({0x5F, InstructionCode(0x5F, "POP")});
_instruction_code_map.insert({0x60, InstructionCode(0x60)});
_instruction_code_map.insert({0x61, InstructionCode(0x61)});
_instruction_code_map.insert({0x62, InstructionCode(0x62)});
_instruction_code_map.insert({0x63, InstructionCode(0x63)});
_instruction_code_map.insert({0x64, InstructionCode(0x64)});
_instruction_code_map.insert({0x65, InstructionCode(0x65)});
_instruction_code_map.insert({0x66, InstructionCode(0x66)});
_instruction_code_map.insert({0x67, InstructionCode(0x67)});
_instruction_code_map.insert({0x68, InstructionCode(0x68)});
_instruction_code_map.insert({0x69, InstructionCode(0x69)});
_instruction_code_map.insert({0x6A, InstructionCode(0x6A)});
_instruction_code_map.insert({0x6B, InstructionCode(0x6B)});
_instruction_code_map.insert({0x6C, InstructionCode(0x6C)});
_instruction_code_map.insert({0x6D, InstructionCode(0x6D)});
_instruction_code_map.insert({0x6E, InstructionCode(0x6E)});
_instruction_code_map.insert({0x6F, InstructionCode(0x6F)});
_instruction_code_map.insert({0x70, InstructionCode(0x70, "JO", "Ib")});        // opcode disp
_instruction_code_map.insert({0x71, InstructionCode(0x71, "JNO", "Ib")});
_instruction_code_map.insert({0x72, InstructionCode(0x72, "JB", "Ib")});
_instruction_code_map.insert({0x73, InstructionCode(0x73, "JNB", "Ib")});
_instruction_code_map.insert({0x74, InstructionCode(0x74, "JZ", "Ib")});
_instruction_code_map.insert({0x75, InstructionCode(0x75, "JNZ", "Ib")});
_instruction_code_map.insert({0x76, InstructionCode(0x76, "JBE", "Ib")});
_instruction_code_map.insert({0x77, InstructionCode(0x77, "JA", "Ib")});
_instruction_code_map.insert({0x78, InstructionCode(0x78, "JS", "Ib")});
_instruction_code_map.insert({0x79, InstructionCode(0x79, "JNS", "Ib")});
_instruction_code_map.insert({0x7A, InstructionCode(0x7A, "JPE", "Ib")});
_instruction_code_map.insert({0x7B, InstructionCode(0x7B, "JPO", "Ib")});
_instruction_code_map.insert({0x7C, InstructionCode(0x7C, "JL", "Ib")});
_instruction_code_map.insert({0x7D, InstructionCode(0x7D, "JGE", "Ib")});
_instruction_code_map.insert({0x7E, InstructionCode(0x7E, "JLE", "Ib")});
_instruction_code_map.insert({0x7F, InstructionCode(0x7F, "JG", "Ib")});
_instruction_code_map.insert({0x80, InstructionCode(0x80, "GRP1", "E", "Ib")});     // opcode mode [disp] [data]
_instruction_code_map.insert({0x81, InstructionCode(0x81, "GRP1", "E", "Iw")});
_instruction_code_map.insert({0x82, InstructionCode(0x82, "GRP1", "E", "Ib")});
_instruction_code_map.insert({0x83, InstructionCode(0x83, "GRP1", "E", "Ib")});
_instruction_code_map.insert({0x84, InstructionCode(0x84, "TEST", "E", "E")});
_instruction_code_map.insert({0x85, InstructionCode(0x85, "TEST", "E", "E")});
_instruction_code_map.insert({0x86, InstructionCode(0x86, "XCHG", "E", "E")});
_instruction_code_map.insert({0x87, InstructionCode(0x87, "XCHG", "E", "E")});
_instruction_code_map.insert({0x88, InstructionCode(0x88, "MOV", "E", "E")});
_instruction_code_map.insert({0x89, InstructionCode(0x89, "MOV", "E", "E")});
_instruction_code_map.insert({0x8A, InstructionCode(0x8A, "MOV", "E", "E")});
_instruction_code_map.insert({0x8B, InstructionCode(0x8B, "MOV", "E", "E")});
_instruction_code_map.insert({0x8C, InstructionCode(0x8C, "MOV", "E", "E")});
_instruction_code_map.insert({0x8D, InstructionCode(0x8D, "LEA", "E")});            // opcode mode [disp]
_instruction_code_map.insert({0x8E, InstructionCode(0x8E, "MOV", "E", "E")});       // opcode mode [disp]
_instruction_code_map.insert({0x8F, InstructionCode(0x8F, "POP", "E")});            // opcode mode
_instruction_code_map.insert({0x90, InstructionCode(0x90, "NOP")});                 // opcode
_instruction_code_map.insert({0x91, InstructionCode(0x91, "XCHG", "E")});           // opcode mode
_instruction_code_map.insert({0x92, InstructionCode(0x92, "XCHG", "E")});
_instruction_code_map.insert({0x93, InstructionCode(0x93, "XCHG", "E")});
_instruction_code_map.insert({0x94, InstructionCode(0x94, "XCHG", "E")});
_instruction_code_map.insert({0x95, InstructionCode(0x95, "XCHG", "E")});
_instruction_code_map.insert({0x96, InstructionCode(0x96, "XCHG", "E")});
_instruction_code_map.insert({0x97, InstructionCode(0x97, "XCHG", "E")});
_instruction_code_map.insert({0x98, InstructionCode(0x98, "CBW")});                 // opcode
_instruction_code_map.insert({0x99, InstructionCode(0x99, "CWD")});                 // opcode
_instruction_code_map.insert({0x9A, InstructionCode(0x9A, "CALL", "E")});           // opcode mode
_instruction_code_map.insert({0x9B, InstructionCode(0x9B, "WAIT")});                // opcode
_instruction_code_map.insert({0x9C, InstructionCode(0x9C, "PUSHF")});               // opcode
_instruction_code_map.insert({0x9D, InstructionCode(0x9D, "POPF")});
_instruction_code_map.insert({0x9E, InstructionCode(0x9E, "SAHF")});
_instruction_code_map.insert({0x9F, InstructionCode(0x9F, "LAHF")});
_instruction_code_map.insert({0xA0, InstructionCode(0xA0, "MOV", "E", "E")});       // opcode mode
_instruction_code_map.insert({0xA1, InstructionCode(0xA1, "MOV", "E", "E")});
_instruction_code_map.insert({0xA2, InstructionCode(0xA2, "MOV", "E", "E")});
_instruction_code_map.insert({0xA3, InstructionCode(0xA3, "MOV", "E", "E")});
_instruction_code_map.insert({0xA4, InstructionCode(0xA4, "MOVSB")});               // opcode
_instruction_code_map.insert({0xA5, InstructionCode(0xA5, "MOVSW")});
_instruction_code_map.insert({0xA6, InstructionCode(0xA6, "CMPSB")});
_instruction_code_map.insert({0xA7, InstructionCode(0xA7, "CMPSW")});
_instruction_code_map.insert({0xA8, InstructionCode(0xA8, "TEST", "E", "Ib")});     // opcode mode
_instruction_code_map.insert({0xA9, InstructionCode(0xA9, "TEST", "E", "Iw")});
_instruction_code_map.insert({0xAA, InstructionCode(0xAA, "STOSB")});               // opcode
_instruction_code_map.insert({0xAB, InstructionCode(0xAB, "STOSW")});
_instruction_code_map.insert({0xAC, InstructionCode(0xAC, "LODSB")});
_instruction_code_map.insert({0xAD, InstructionCode(0xAD, "LODSW")});
_instruction_code_map.insert({0xAE, InstructionCode(0xAE, "SCASB")});
_instruction_code_map.insert({0xAF, InstructionCode(0xAF, "SCASW")});
_instruction_code_map.insert({0xB0, InstructionCode(0xB0, "MOV", "E", "Ib")});      // opcode mode [disp] data
_instruction_code_map.insert({0xB1, InstructionCode(0xB1, "MOV", "E", "Ib")});
_instruction_code_map.insert({0xB2, InstructionCode(0xB2, "MOV", "E", "Ib")});
_instruction_code_map.insert({0xB3, InstructionCode(0xB3, "MOV", "E", "Ib")});
_instruction_code_map.insert({0xB4, InstructionCode(0xB4, "MOV", "E", "Ib")});
_instruction_code_map.insert({0xB5, InstructionCode(0xB5, "MOV", "E", "Ib")});
_instruction_code_map.insert({0xB6, InstructionCode(0xB6, "MOV", "E", "Ib")});
_instruction_code_map.insert({0xB7, InstructionCode(0xB7, "MOV", "E", "Ib")});
_instruction_code_map.insert({0xB8, InstructionCode(0xB8, "MOV", "E", "Iw")});
_instruction_code_map.insert({0xB9, InstructionCode(0xB9, "MOV", "E", "Iw")});
_instruction_code_map.insert({0xBA, InstructionCode(0xBA, "MOV", "E", "Iw")});
_instruction_code_map.insert({0xBB, InstructionCode(0xBB, "MOV", "E", "Iw")});
_instruction_code_map.insert({0xBC, InstructionCode(0xBC, "MOV", "E", "Iw")});
_instruction_code_map.insert({0xBD, InstructionCode(0xBD, "MOV", "E", "Iw")});
_instruction_code_map.insert({0xBE, InstructionCode(0xBE, "MOV", "E", "Iw")});
_instruction_code_map.insert({0xBF, InstructionCode(0xBF, "MOV", "E", "Iw")});
_instruction_code_map.insert({0xC0, InstructionCode(0xC0)});
_instruction_code_map.insert({0xC1, InstructionCode(0xC1)});
_instruction_code_map.insert({0xC2, InstructionCode(0xC2, "RET", "Iw")});           // opcode data
_instruction_code_map.insert({0xC3, InstructionCode(0xC3, "RET")});                 // opcode
_instruction_code_map.insert({0xC4, InstructionCode(0xC4, "LES", "E", "E")});       // opcode mode
_instruction_code_map.insert({0xC5, InstructionCode(0xC5, "LDS", "E", "E")});
_instruction_code_map.insert({0xC6, InstructionCode(0xC6, "MOV", "E", "Ib")});      // opcode mode [disp] data
_instruction_code_map.insert({0xC7, InstructionCode(0xC7, "MOV", "E", "Iw")});
_instruction_code_map.insert({0xC8, InstructionCode(0xC8)});
_instruction_code_map.insert({0xC9, InstructionCode(0xC9)});
_instruction_code_map.insert({0xCA, InstructionCode(0xCA, "RETF", "Iw")});          // opcode data
_instruction_code_map.insert({0xCB, InstructionCode(0xCB, "RETF")});                // opcode
_instruction_code_map.insert({0xCC, InstructionCode(0xCC, "INT", "3")});            // opcode data
_instruction_code_map.insert({0xCD, InstructionCode(0xCD, "INT", "Ib")});
_instruction_code_map.insert({0xCE, InstructionCode(0xCE, "INTO")});                // opcode
_instruction_code_map.insert({0xCF, InstructionCode(0xCF, "IRET")});                // opcode
_instruction_code_map.insert({0xD0, InstructionCode(0xD0, "GRP2", "E", "1")});     // opcode data
_instruction_code_map.insert({0xD1, InstructionCode(0xD1, "GRP2", "E", "1")});
_instruction_code_map.insert({0xD2, InstructionCode(0xD2, "GRP2", "E", "E")});      // opcode mode
_instruction_code_map.insert({0xD3, InstructionCode(0xD3, "GRP2", "E", "E")});
_instruction_code_map.insert({0xD4, InstructionCode(0xD4, "AAM", "I0")});           // ?
_instruction_code_map.insert({0xD5, InstructionCode(0xD5, "AAD", "I0")});
_instruction_code_map.insert({0xD6, InstructionCode(0xD6)});
_instruction_code_map.insert({0xD7, InstructionCode(0xD7, "XLAT")});                // opcode
_instruction_code_map.insert({0xD8, InstructionCode(0xD8)});
_instruction_code_map.insert({0xD9, InstructionCode(0xD9)});
_instruction_code_map.insert({0xDA, InstructionCode(0xDA)});
_instruction_code_map.insert({0xDB, InstructionCode(0xDB)});
_instruction_code_map.insert({0xDC, InstructionCode(0xDC)});
_instruction_code_map.insert({0xDD, InstructionCode(0xDD)});
_instruction_code_map.insert({0xDE, InstructionCode(0xDE)});
_instruction_code_map.insert({0xDF, InstructionCode(0xDF)});
_instruction_code_map.insert({0xE0, InstructionCode(0xE0, "LOOPNZ", "Ib")});      // opcode data
_instruction_code_map.insert({0xE1, InstructionCode(0xE1, "LOOPZ", "Ib")});
_instruction_code_map.insert({0xE2, InstructionCode(0xE2, "LOOP", "Ib")});
_instruction_code_map.insert({0xE3, InstructionCode(0xE3, "JCXZ", "Ib")});
_instruction_code_map.insert({0xE4, InstructionCode(0xE4, "IN", "E", "Ib")});     // opcode mode data
_instruction_code_map.insert({0xE5, InstructionCode(0xE5, "IN", "E", "Ib")});
_instruction_code_map.insert({0xE6, InstructionCode(0xE6, "OUT", "Ib", "E")});    // opcode mode data
_instruction_code_map.insert({0xE7, InstructionCode(0xE7, "OUT", "Ib", "E")});
_instruction_code_map.insert({0xE8, InstructionCode(0xE8, "CALL", "Iv")});        // opcode data
_instruction_code_map.insert({0xE9, InstructionCode(0xE9, "JMP", "Iv")});         // opcode data
_instruction_code_map.insert({0xEA, InstructionCode(0xEA, "JMP", "Iv")});
_instruction_code_map.insert({0xEB, InstructionCode(0xEB, "JMP", "Ib")});
_instruction_code_map.insert({0xEC, InstructionCode(0xEC, "IN", "E", "E")});      // opcode mode
_instruction_code_map.insert({0xED, InstructionCode(0xED, "IN", "E", "E")});
_instruction_code_map.insert({0xEE, InstructionCode(0xEE, "OUT", "E", "E")});
_instruction_code_map.insert({0xEF, InstructionCode(0xEF, "OUT", "E", "E")});
_instruction_code_map.insert({0xF0, InstructionCode(0xF0, "LOCK")});              // opcode
_instruction_code_map.insert({0xF1, InstructionCode(0xF1)});
_instruction_code_map.insert({0xF2, InstructionCode(0xF2, "REPNZ")});
_instruction_code_map.insert({0xF3, InstructionCode(0xF3, "REPZ")});
_instruction_code_map.insert({0xF4, InstructionCode(0xF4, "HLT")});
_instruction_code_map.insert({0xF5, InstructionCode(0xF5, "CMC")});
_instruction_code_map.insert({0xF6, InstructionCode(0xF6, "GRP3a", "E")});
_instruction_code_map.insert({0xF7, InstructionCode(0xF7, "GRP3b", "E")});
_instruction_code_map.insert({0xF8, InstructionCode(0xF8, "CLC")});
_instruction_code_map.insert({0xF9, InstructionCode(0xF9, "STC")});
_instruction_code_map.insert({0xFA, InstructionCode(0xFA, "CLI")});
_instruction_code_map.insert({0xFB, InstructionCode(0xFB, "STI")});
_instruction_code_map.insert({0xFC, InstructionCode(0xFC, "CLD")});
_instruction_code_map.insert({0xFD, InstructionCode(0xFD, "STD")});
_instruction_code_map.insert({0xFE, InstructionCode(0xFE, "GRP4", "E")});
_instruction_code_map.insert({0xFF, InstructionCode(0xFF, "GRP5", "E")});
}

InstructionSet::create_func_t InstructionSet::find(const MicroOp::Key &key) {
  auto it =
      std::find_if(instructions.begin(), instructions.end(),
                   [&key](const std::pair<MicroOp::Key, create_func_t> &rhs) {
                     return rhs.first == key;
                   });
  if (it == std::end(instructions)) {
    assert(false);
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
  auto instruction_code = find(opcode);
  if(instruction_code == nullptr){
    //1. A loud complain
    assert(false);
    //
    //2.
    //Return a NOP instruction
    //return find(MicroOp::Key(0x90))(params);
  }
  return find(MicroOp::Key(instruction_code->_opcode))(params);
}
