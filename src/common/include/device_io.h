//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _DEVICE_IO_H_
#define _DEVICE_IO_H_

#include "address.h"
#include "bytes.h"

struct DeviceIO {
  virtual ~DeviceIO() = default;
  virtual uint16_t write(Address const *, const Bytes &) = 0;
  virtual Bytes read(Address const *, uint16_t size) = 0;
  virtual std::streambuf *rdbuf() { return nullptr; }
  uint16_t read_u16(Address const *);
  uint8_t read_u8(Address const *);
};

#endif // _DEVICE_IO_H_
