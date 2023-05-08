#include "execution_unit.h"

ExecutionUnit::ExecutionUnit(bus_ptr_t bus, registers_ptr_t registers)
    : _buf(bus->rdbuf()), _params(bus, registers) {}

void ExecutionUnit::fetch_decode_execute() {
  seek(_params.registers->IP);
  auto args = fetch();
  decode(args.first)->execute(args.second);
}

void ExecutionUnit::bootstrap(std::streambuf *program, bool replace) {
  if (replace) {
    _buf = program;
  } else {
    const auto _size = 512;
    uint8_t _chr[_size] = {};
    uint16_t _len = 0;
    while (program->pubseekoff(_len, std::ios_base::cur, std::ios_base::out) !=
           -1) {
      _len = program->sgetn(reinterpret_cast<char *>(_chr), _size); // NOLINT
      _buf->sputn(reinterpret_cast<const char *>(_chr), _len);      // NOLINT
    }
  }
  // TODO set up registers
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
  auto _code = _instruction_set.find(_opcode);
  if (_code->_arguments.empty()) {
    PLOGD << fmt::format("memonic={}, sop={:x},opcode={:x}", _code->_memonic,
                         _sop, _opcode);
    return {_opcode, Instruction(_sop, _opcode)};
  }
  uint8_t _mode = 0;
  if (_code->has_mode()) {
    _mode = getb();
    auto _has_offset = _code->has_disp(_mode);
    if (_has_offset.first) {
      auto _offset_len = _has_offset.second;
      _offset = _offset_len == sizeof(uint8_t) ? getb() : getw();
    }
  }
  uint16_t _opcode_mode = make_word(_opcode, _mode);
  auto _has_data = _code->has_data();
  if (_has_data.first) {
    _data_len = _has_data.second;
    _data = _data_len == sizeof(uint8_t) ? getb() : getw();
  }
  auto _instruction =
      _data_len == sizeof(uint8_t)
          ? Instruction(_sop, _opcode_mode, _offset, (uint8_t)_data)
          : Instruction(_sop, _opcode_mode, _offset, (uint16_t)_data);
  PLOGD << fmt::format("memonic={}", _code->_memonic) << _instruction;
  return {_opcode, _instruction};
}

std::shared_ptr<MicroOp> ExecutionUnit::decode(uint8_t opcode) {
  return _instruction_set.decode(opcode, _params);
}

uint16_t ExecutionUnit::beg() {
  auto res = _buf->pubseekpos((uint16_t)_params.registers->IP);
  return res;
}

bool ExecutionUnit::eof() const {
  return _buf->pubseekoff(0, std::ios_base::cur) == -1;
}

uint16_t ExecutionUnit::seek(uint16_t pos) { return _buf->pubseekpos(pos); }

uint8_t ExecutionUnit::getb() { return _get<uint8_t>(); }

uint16_t ExecutionUnit::getw() { return _get<uint16_t>(); }
