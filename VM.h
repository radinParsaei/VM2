#ifndef VM_H
#define VM_H
#include "opcodes.h"
#ifdef __AVR__
#define USE_DOUBLE
// #define USE_BIG_NUMBER
#define USE_ARDUINO_STRING
#define USE_ARDUINO_ARRAY
#define USE_NOSTD_MAP
#define MAX_FIXED_ARRAY_SIZE 100
#else
#define VECTOR_RESERVED_SIZE 1000
#endif
#if !__has_include("Arduino.h")
#include <iostream>
#endif
#include "value.h"

#define NEEDS_PARAMETER(opcode) opcode == OPCODE_PUT || opcode == OPCODE_GETVAR || opcode == OPCODE_SETVAR

class VM {
private:
    Value stack = Types::Array;
    Value mem = Types::Map;
    short rec = 0;

public:
    VM();
    bool run1(int opcode, const Value& data = Types::Null); // returns true if the data is used
    void run(const Value& program);
};
#endif // OPCODES_H