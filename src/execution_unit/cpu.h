/*
 * CPU.h
 *
 */

#ifndef CPU_H_
#define CPU_H_

#include "types.h"

#include <cstdint>
#include <vector>
#include <string>


class CPU {
    BUS *_ram;
    Decoder* _decoder;
public:
    CPU(BUS*, Decoder*);
    virtual ~CPU();
    void start();
    void reset();
};

#endif /* CPU_H_ */
