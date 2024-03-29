#include "execution_unit.h"

ExecutionUnit::ExecutionUnit(bus_ptr_t bus, registers_ptr_t registers)
    : _params(bus, registers) {}

void ExecutionUnit::fetch_decode_execute() {
  seek(_params.registers->IP);
  auto args = fetch();
  decode(args.first)->execute(args.second);
}

void ExecutionUnit::bootstrap(std::streambuf *program) {
  _params.bus->devices().initialize(this);
  _buf = program;
  beg();
}

void ExecutionUnit::interrupt(uint8_t type) { _interrupts.push(type); }

std::pair<uint8_t, Instruction> ExecutionUnit::fetch() {
  uint8_t _sop = SOP::NONE, _opcode = 0;
  uint16_t _offset = 0, _data = 0; // offset == displacement
  auto _data_len = 0;
  _sop = getb();
  if (SOP::is_sop(_sop)) {
    _opcode = getb();
  } else {
    _opcode = _sop;
    _sop = SOP::NONE;
  }
  auto _code = _instruction_unit.find(_opcode);
  if (_code->_arguments.empty()) {
    PLOGD << fmt::format("memonic={}, sop={:x},opcode={:x}", _code->_memonic,
                         _sop, _opcode);
    return {_opcode, Instruction(_sop, _opcode)};
  }
  uint8_t _mod = 0;
  if (_code->has_mod()) {
    _mod = getb();
    auto _has_offset = _code->has_disp(_mod);
    if (_has_offset.first) {
      auto _offset_len = _has_offset.second;
      _offset = _offset_len == sizeof_ui8 ? getb() : getw();
    }
  }
  uint16_t _opcode_mod = make_word(_opcode, _mod);
  auto _has_data = _code->has_data();
  if (_has_data.first) {
    _data_len = _has_data.second;
    _data = _data_len == sizeof_ui8 ? getb() : getw();
  }
  auto _instruction =
      _data_len == sizeof_ui8
          ? Instruction(_sop, _opcode_mod, _offset, cast_ui8(_data))
          : Instruction(_sop, _opcode_mod, _offset, cast_ui16(_data));
  PLOGD << fmt::format("memonic={}", _code->_memonic) << _instruction;
  return {_opcode, _instruction};
}

std::shared_ptr<MicroOp> ExecutionUnit::decode(uint8_t opcode) {
  return _instruction_unit.decode(opcode, _params);
}

uint16_t ExecutionUnit::beg() {
  auto res = _buf->pubseekpos(cast_ui16(_params.registers->IP));
  return res;
}

bool ExecutionUnit::eof() const {
  return _buf->pubseekoff(0, std::ios_base::cur) == -1;
}

uint16_t ExecutionUnit::seek(uint16_t pos) { return _buf->pubseekpos(pos); }

uint8_t ExecutionUnit::getb() { return _get<uint8_t>(); }

uint16_t ExecutionUnit::getw() { return _get<uint16_t>(); }
