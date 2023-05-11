
#ifndef _BUS_H_
#define _BUS_H_

#include "address.h"
#include "bytes.h"

#include <cstdint>
#include <streambuf>

struct BUS {
  virtual ~BUS() = default;
  virtual uint16_t write(Address const *, const Bytes &) = 0;
  virtual Bytes read(Address const *, uint16_t size) = 0;
  virtual std::streambuf *rdbuf() { return nullptr; }
};

#endif // _BUS_H_
