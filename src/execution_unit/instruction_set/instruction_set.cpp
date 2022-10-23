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

#define _INSTRUCTION(opcode, memonic, argument_code, cls)                      \
  register_instruction<cls>(MicroOp::Key(opcode, memonic, argument_code))

InstructionSet::InstructionSet() {
  /*_INSTRUCTION(0x88, 0xFC, "MOV", MovRegisterAndMemory);
  _INSTRUCTION(0xB0, 0xF0, "MOV", MovRegisterImmediate);
  _INSTRUCTION(0xA0, 0xF0, "MOV", MovAccumulator);
  _INSTRUCTION(0x8E, 0xFC, "MOV", MovSegmentAndRegisterMemory);
  _INSTRUCTION(0xC6, 0xFE, "MOV", MovMemoryImmediate);
  _INSTRUCTION(0x50, 0xF0, "PUSH", PushRegister);
  _INSTRUCTION(0xFF, 0xFF, "PUSH", PushMemory);
  _INSTRUCTION(0x06, 0xE7, "PUSH", PushSegment);
  // _INSTRUCTION(0x9C, 0xFF, "PUSH", PushFlags);
  _INSTRUCTION(0x58, 0xF8, "POP", PopRegister);
  _INSTRUCTION(0x8F, 0xFF, "POP", PopMemory);
  _INSTRUCTION(0x07, 0xE7, "POP", PopSegment);
  // _INSTRUCTION(0x9D, 0xFF, "POP", PopFlags);
  _INSTRUCTION(0x90, 0xF8, "XCHG", XCHGAccumulator);
  _INSTRUCTION(0x86, 0xFE, "XCHG", XCHGRegisterMemory);
  _INSTRUCTION(0xE4, 0xFE, "IN", INPort);
  _INSTRUCTION(0xEC, 0xFE, "IN", INDX);
  _INSTRUCTION(0xE6, 0xFE, "OUT", OUTPort);
  _INSTRUCTION(0xEE, 0xFE, "OUT", OUTDX);
  _INSTRUCTION(0xD7, 0xD6, "XLAT", XLAT);
  _INSTRUCTION(0x8D, 0xFF, "LEA", LEA);
  _INSTRUCTION(0xC4, 0xFF, "LES", LES);
  _INSTRUCTION(0xC5, 0xFF, "LDS", LDS);
  _INSTRUCTION(0x9F, 0xFF, "LAHF", LAHF);
  _INSTRUCTION(0x9E, 0xFF, "SAHF", SAHF);*/

  _INSTRUCTION(0x37, "AAA", "", NoOp);
  _INSTRUCTION(0xD5, "AAD", "", NoOp);
  _INSTRUCTION(0xD4, "AAM", "", NoOp);
  _INSTRUCTION(0x3F, "AAS", "", NoOp);
  _INSTRUCTION(0x10, "ADC", "", NoOp);
  _INSTRUCTION(0x11, "ADC", "", NoOp);
  _INSTRUCTION(0x12, "ADC", "", NoOp);
  _INSTRUCTION(0x13, "ADC", "", NoOp);
  _INSTRUCTION(0x14, "ADC", "", NoOp);
  _INSTRUCTION(0x15, "ADC", "", NoOp);
  _INSTRUCTION(0x00, "ADD", "", NoOp);
  _INSTRUCTION(0x01, "ADD", "", NoOp);
  _INSTRUCTION(0x02, "ADD", "", NoOp);
  _INSTRUCTION(0x03, "ADD", "", NoOp);
  _INSTRUCTION(0x04, "ADD", "", NoOp);
  _INSTRUCTION(0x05, "ADD", "", NoOp);
  _INSTRUCTION(0x20, "AND", "", NoOp);
  _INSTRUCTION(0x21, "AND", "", NoOp);
  _INSTRUCTION(0x22, "AND", "", NoOp);
  _INSTRUCTION(0x23, "AND", "", NoOp);
  _INSTRUCTION(0x24, "AND", "", NoOp);
  _INSTRUCTION(0x25, "AND", "", NoOp);
  _INSTRUCTION(0x9A, "CALL", "", NoOp);
  _INSTRUCTION(0xE8, "CALL", "", NoOp);
  _INSTRUCTION(0x98, "CBW", "", NoOp);
  _INSTRUCTION(0xF8, "CLC", "", NoOp);
  _INSTRUCTION(0xFC, "CLD", "", NoOp);
  _INSTRUCTION(0xFA, "CLI", "", NoOp);
  _INSTRUCTION(0xF5, "CMC", "", NoOp);
  _INSTRUCTION(0x38, "CMP", "", NoOp);
  _INSTRUCTION(0x39, "CMP", "", NoOp);
  _INSTRUCTION(0x3A, "CMP", "", NoOp);
  _INSTRUCTION(0x3B, "CMP", "", NoOp);
  _INSTRUCTION(0x3C, "CMP", "", NoOp);
  _INSTRUCTION(0x3D, "CMP", "", NoOp);
  _INSTRUCTION(0xA6, "CMPSB", "", NoOp);
  _INSTRUCTION(0xA7, "CMPSW", "", NoOp);
  _INSTRUCTION(0x2E, "CS", "", NoOp);
  _INSTRUCTION(0x99, "CWD", "", NoOp);
  _INSTRUCTION(0x27, "DAA", "", NoOp);
  _INSTRUCTION(0x2F, "DAS", "", NoOp);
  _INSTRUCTION(0x48, "DEC", "", NoOp);
  _INSTRUCTION(0x49, "DEC", "", NoOp);
  _INSTRUCTION(0x4A, "DEC", "", NoOp);
  _INSTRUCTION(0x4B, "DEC", "", NoOp);
  _INSTRUCTION(0x4C, "DEC", "", NoOp);
  _INSTRUCTION(0x4D, "DEC", "", NoOp);
  _INSTRUCTION(0x4E, "DEC", "", NoOp);
  _INSTRUCTION(0x4F, "DEC", "", NoOp);
  _INSTRUCTION(0x3E, "DS", "", NoOp);
  _INSTRUCTION(0x26, "ES", "", NoOp);
  _INSTRUCTION(0x80, "GRP1", "", NoOp);
  _INSTRUCTION(0x81, "GRP1", "", NoOp);
  _INSTRUCTION(0x82, "GRP1", "", NoOp);
  _INSTRUCTION(0x83, "GRP1", "", NoOp);
  _INSTRUCTION(0xD0, "GRP2", "", NoOp);
  _INSTRUCTION(0xD1, "GRP2", "", NoOp);
  _INSTRUCTION(0xD2, "GRP2", "", NoOp);
  _INSTRUCTION(0xD3, "GRP2", "", NoOp);
  _INSTRUCTION(0xF6, "GRP3a", "", NoOp);
  _INSTRUCTION(0xF7, "GRP3b", "", NoOp);
  _INSTRUCTION(0xFE, "GRP4", "", NoOp);
  _INSTRUCTION(0xFF, "GRP5", "", NoOp);
  _INSTRUCTION(0xF4, "HLT", "", NoOp);
  _INSTRUCTION(0xE4, "IN", "", NoOp);
  _INSTRUCTION(0xE5, "IN", "", NoOp);
  _INSTRUCTION(0xEC, "IN", "", NoOp);
  _INSTRUCTION(0xED, "IN", "", NoOp);
  _INSTRUCTION(0x40, "INC", "", NoOp);
  _INSTRUCTION(0x41, "INC", "", NoOp);
  _INSTRUCTION(0x42, "INC", "", NoOp);
  _INSTRUCTION(0x43, "INC", "", NoOp);
  _INSTRUCTION(0x44, "INC", "", NoOp);
  _INSTRUCTION(0x45, "INC", "", NoOp);
  _INSTRUCTION(0x46, "INC", "", NoOp);
  _INSTRUCTION(0x47, "INC", "", NoOp);
  _INSTRUCTION(0xCC, "INT", "", NoOp);
  _INSTRUCTION(0xCD, "INT", "", NoOp);
  _INSTRUCTION(0xCE, "INTO", "", NoOp);
  _INSTRUCTION(0xCF, "IRET", "", NoOp);
  _INSTRUCTION(0x77, "JA", "", NoOp);
  _INSTRUCTION(0x72, "JB", "", NoOp);
  _INSTRUCTION(0x76, "JBE", "", NoOp);
  _INSTRUCTION(0xE3, "JCXZ", "", NoOp);
  _INSTRUCTION(0x7F, "JG", "", NoOp);
  _INSTRUCTION(0x7D, "JGE", "", NoOp);
  _INSTRUCTION(0x7C, "JL", "", NoOp);
  _INSTRUCTION(0x7E, "JLE", "", NoOp);
  _INSTRUCTION(0xE9, "JMP", "", NoOp);
  _INSTRUCTION(0xEA, "JMP", "", NoOp);
  _INSTRUCTION(0xEB, "JMP", "", NoOp);
  _INSTRUCTION(0x73, "JNB", "", NoOp);
  _INSTRUCTION(0x71, "JNO", "", NoOp);
  _INSTRUCTION(0x79, "JNS", "", NoOp);
  _INSTRUCTION(0x75, "JNZ", "", NoOp);
  _INSTRUCTION(0x70, "JO", "", NoOp);
  _INSTRUCTION(0x7A, "JPE", "", NoOp);
  _INSTRUCTION(0x7B, "JPO", "", NoOp);
  _INSTRUCTION(0x78, "JS", "", NoOp);
  _INSTRUCTION(0x74, "JZ", "", NoOp);
  _INSTRUCTION(0x9F, "LAHF", "", NoOp);
  _INSTRUCTION(0xC5, "LDS", "", NoOp);
  _INSTRUCTION(0x8D, "LEA", "", NoOp);
  _INSTRUCTION(0xC4, "LES", "", NoOp);
  _INSTRUCTION(0xF0, "LOCK", "", NoOp);
  _INSTRUCTION(0xAC, "LODSB", "", NoOp);
  _INSTRUCTION(0xAD, "LODSW", "", NoOp);
  _INSTRUCTION(0xE2, "LOOP", "", NoOp);
  _INSTRUCTION(0xE0, "LOOPNZ", "", NoOp);
  _INSTRUCTION(0xE1, "LOOPZ", "", NoOp);
  _INSTRUCTION(0x88, "MOV", "Ew", MovRegisterAndMemory);
  _INSTRUCTION(0x89, "MOV", "Ew", MovRegisterAndMemory);
  _INSTRUCTION(0x8A, "MOV", "Ew", MovRegisterAndMemory);
  _INSTRUCTION(0x8B, "MOV", "Ew", MovRegisterAndMemory);
  _INSTRUCTION(0x8C, "MOV", "", NoOp);
  _INSTRUCTION(0x8E, "MOV", "", NoOp);
  _INSTRUCTION(0xA0, "MOV", "", NoOp);
  _INSTRUCTION(0xA1, "MOV", "", NoOp);
  _INSTRUCTION(0xA2, "MOV", "", NoOp);
  _INSTRUCTION(0xA3, "MOV", "", NoOp);
  _INSTRUCTION(0xB0, "MOV", "", NoOp);
  _INSTRUCTION(0xB1, "MOV", "", NoOp);
  _INSTRUCTION(0xB2, "MOV", "", NoOp);
  _INSTRUCTION(0xB3, "MOV", "", NoOp);
  _INSTRUCTION(0xB4, "MOV", "", NoOp);
  _INSTRUCTION(0xB5, "MOV", "", NoOp);
  _INSTRUCTION(0xB6, "MOV", "", NoOp);
  _INSTRUCTION(0xB7, "MOV", "", NoOp);
  _INSTRUCTION(0xB8, "MOV", "", NoOp);
  _INSTRUCTION(0xB9, "MOV", "", NoOp);
  _INSTRUCTION(0xBA, "MOV", "", NoOp);
  _INSTRUCTION(0xBB, "MOV", "", NoOp);
  _INSTRUCTION(0xBC, "MOV", "", NoOp);
  _INSTRUCTION(0xBD, "MOV", "", NoOp);
  _INSTRUCTION(0xBE, "MOV", "", NoOp);
  _INSTRUCTION(0xBF, "MOV", "", NoOp);
  _INSTRUCTION(0xC6, "MOV", "", NoOp);
  _INSTRUCTION(0xC7, "MOV", "", NoOp);

  _INSTRUCTION(0xA4, "MOVSB", "", NoOp);
  _INSTRUCTION(0xA5, "MOVSW", "", NoOp);
  _INSTRUCTION(0x90, "NOP", "", NoOp);
  _INSTRUCTION(0x08, "OR", "", NoOp);
  _INSTRUCTION(0x09, "OR", "", NoOp);
  _INSTRUCTION(0x0A, "OR", "", NoOp);
  _INSTRUCTION(0x0B, "OR", "", NoOp);
  _INSTRUCTION(0x0C, "OR", "", NoOp);
  _INSTRUCTION(0x0D, "OR", "", NoOp);
  _INSTRUCTION(0xE6, "OUT", "", NoOp);
  _INSTRUCTION(0xE7, "OUT", "", NoOp);
  _INSTRUCTION(0xEE, "OUT", "", NoOp);
  _INSTRUCTION(0xEF, "OUT", "", NoOp);
  _INSTRUCTION(0x07, "POP", "", NoOp);
  _INSTRUCTION(0x17, "POP", "", NoOp);
  _INSTRUCTION(0x1F, "POP", "", NoOp);
  _INSTRUCTION(0x58, "POP", "", NoOp);
  _INSTRUCTION(0x59, "POP", "", NoOp);
  _INSTRUCTION(0x5A, "POP", "", NoOp);
  _INSTRUCTION(0x5B, "POP", "", NoOp);
  _INSTRUCTION(0x5C, "POP", "", NoOp);
  _INSTRUCTION(0x5D, "POP", "", NoOp);
  _INSTRUCTION(0x5E, "POP", "", NoOp);
  _INSTRUCTION(0x5F, "POP", "", NoOp);
  _INSTRUCTION(0x8F, "POP", "", NoOp);
  _INSTRUCTION(0x9D, "POPF", "", NoOp);
  _INSTRUCTION(0x06, "PUSH", "", NoOp);
  _INSTRUCTION(0x0E, "PUSH", "", NoOp);
  _INSTRUCTION(0x16, "PUSH", "", NoOp);
  _INSTRUCTION(0x1E, "PUSH", "", NoOp);
  _INSTRUCTION(0x50, "PUSH", "", NoOp);
  _INSTRUCTION(0x51, "PUSH", "", NoOp);
  _INSTRUCTION(0x52, "PUSH", "", NoOp);
  _INSTRUCTION(0x53, "PUSH", "", NoOp);
  _INSTRUCTION(0x54, "PUSH", "", NoOp);
  _INSTRUCTION(0x55, "PUSH", "", NoOp);
  _INSTRUCTION(0x56, "PUSH", "", NoOp);
  _INSTRUCTION(0x57, "PUSH", "", NoOp);
  _INSTRUCTION(0x9C, "PUSHF", "", NoOp);
  _INSTRUCTION(0xF2, "REPNZ", "", NoOp);
  _INSTRUCTION(0xF3, "REPZ", "", NoOp);
  _INSTRUCTION(0xC2, "RET", "", NoOp);
  _INSTRUCTION(0xC3, "RET", "", NoOp);
  _INSTRUCTION(0xCA, "RETF", "", NoOp);
  _INSTRUCTION(0xCB, "RETF", "", NoOp);
  _INSTRUCTION(0x9E, "SAHF", "", NoOp);
  _INSTRUCTION(0x18, "SBB", "", NoOp);
  _INSTRUCTION(0x19, "SBB", "", NoOp);
  _INSTRUCTION(0x1A, "SBB", "", NoOp);
  _INSTRUCTION(0x1B, "SBB", "", NoOp);
  _INSTRUCTION(0x1C, "SBB", "", NoOp);
  _INSTRUCTION(0x1D, "SBB", "", NoOp);
  _INSTRUCTION(0xAE, "SCASB", "", NoOp);
  _INSTRUCTION(0xAF, "SCASW", "", NoOp);
  _INSTRUCTION(0x36, "SS", "", NoOp);
  _INSTRUCTION(0xF9, "STC", "", NoOp);
  _INSTRUCTION(0xFD, "STD", "", NoOp);
  _INSTRUCTION(0xFB, "STI", "", NoOp);
  _INSTRUCTION(0xAA, "STOSB", "", NoOp);
  _INSTRUCTION(0xAB, "STOSW", "", NoOp);
  _INSTRUCTION(0x28, "SUB", "", NoOp);
  _INSTRUCTION(0x29, "SUB", "", NoOp);
  _INSTRUCTION(0x2A, "SUB", "", NoOp);
  _INSTRUCTION(0x2B, "SUB", "", NoOp);
  _INSTRUCTION(0x2C, "SUB", "", NoOp);
  _INSTRUCTION(0x2D, "SUB", "", NoOp);
  _INSTRUCTION(0x84, "TEST", "", NoOp);
  _INSTRUCTION(0x85, "TEST", "", NoOp);
  _INSTRUCTION(0xA8, "TEST", "", NoOp);
  _INSTRUCTION(0xA9, "TEST", "", NoOp);
  _INSTRUCTION(0x9B, "WAIT", "", NoOp);
  _INSTRUCTION(0x86, "XCHG", "", NoOp);
  _INSTRUCTION(0x87, "XCHG", "", NoOp);
  _INSTRUCTION(0x91, "XCHG", "", NoOp);
  _INSTRUCTION(0x92, "XCHG", "", NoOp);
  _INSTRUCTION(0x93, "XCHG", "", NoOp);
  _INSTRUCTION(0x94, "XCHG", "", NoOp);
  _INSTRUCTION(0x95, "XCHG", "", NoOp);
  _INSTRUCTION(0x96, "XCHG", "", NoOp);
  _INSTRUCTION(0x97, "XCHG", "", NoOp);
  _INSTRUCTION(0xD7, "XLAT", "", NoOp);
  _INSTRUCTION(0x30, "XOR", "", NoOp);
  _INSTRUCTION(0x31, "XOR", "", NoOp);
  _INSTRUCTION(0x32, "XOR", "", NoOp);
  _INSTRUCTION(0x33, "XOR", "", NoOp);
  _INSTRUCTION(0x34, "XOR", "", NoOp);
  _INSTRUCTION(0x35, "XOR", "", NoOp);
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

std::shared_ptr<MicroOp> InstructionSet::decode(uint8_t opcode,
                                                const MicroOp::Params &params) {
  return find(MicroOp::Key(opcode))(params);
}