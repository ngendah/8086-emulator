//
// Copyright (c) 2023 Ngenda Henry
//
// For the license information refer to LICENSE.

#ifndef _FLAGS_H_
#define _FLAGS_H_

#include "io.h"
#include "types.h"

struct Flags final : IO {
  Flags();

  Flags(const Flags &rhs);

  virtual ~Flags(){};

  void set(uint16_t flags);

  template <typename T> T bits() const { return *(T *)&_flags.word; }

  template<typename T> T* to_bits() { return (T *)&_flags.word; }

  operator uint16_t() const;

  friend std::ostream &operator<<(UNUSED_PARAM std::ostream &ostream,
                                  UNUSED_PARAM const Flags &);

  void write_hi(const uint8_t val) override;

  void write_lo(const uint8_t val) override;

  void write(const uint16_t val) override;

  uint16_t read() const override;

  uint8_t read_hi() const override;

  uint8_t read_lo() const override;

protected:
  void write(UNUSED_PARAM const uint8_t) override;

  uint8_t read_byte() const override;

  u16_t _flags;
};

#endif // _FLAGS_H_
