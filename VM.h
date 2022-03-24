#ifndef VM_H
#define VM_H
#include "opcodes.h"
#ifdef __AVR__
#define USE_DOUBLE
// #define USE_BIG_NUMBER
#define USE_ARDUINO_STRING
#define USE_ARDUINO_ARRAY
#define USE_NOSTD_MAP
#define MAX_FIXED_ARRAY_SIZE 10
#else
#define VECTOR_RESERVED_SIZE 1000
#endif
#if !__has_include("Arduino.h")
#include <iostream>
#include <array>
#include <map>
#endif
#include "value.h"

#define FLOW_CONTROL_CONTINUE       0b00000001
#define FLOW_CONTROL_BREAK          0b00000010
#define FLOW_CONTROL_RETURN         0b00000100

#define NEEDS_PARAMETER(opcode) opcode == OPCODE_PUT || opcode == OPCODE_GETVAR || opcode == OPCODE_SETVAR || opcode == OPCODE_CREATE_ARR || opcode == OPCODE_CREATE_MAP || opcode == OPCODE_INCREASE || opcode == OPCODE_MKFUNC || opcode == OPCODE_CALLFUNC || opcode == OPCODE_GETPARAM || opcode == OPCODE_SKIPIFN || opcode == OPCODE_SKIP

class VM {
private:
    void _run_program_from_unsigned_long_array(unsigned long array[], const unsigned int& len, void** toFree = 0, unsigned short* toFreeCount = 0);
    void _recorded_program_to_unsigned_long_array(unsigned long array[], const Value& prog, const int& progLength, const bool clone_ = false);
#ifndef USE_ARDUINO_ARRAY
    std::unordered_map<Value, unsigned long*, HashFunction> functions;
    std::vector<Value*> valuesToFree;
    std::array<Value, 255> stack;
    char top = 0;
#define stackTOP stack[top - 1]
#define _POP_ top --;
#else
    Value functions = Value(Types::Map);
    Value stack = Types::Array;
#define stackTOP stack[stack.length() - 1]
#define _POP_ stack._pop();
#endif
    Value mem = Types::Map;
    unsigned char rec = 0;
    Value params = Types::Array; // holds function parameters
    char flowControlFlags = 0;
    unsigned short skip = 0;
public:
    VM();
    bool run1(int opcode, const Value& data = Types::Null); // returns true if the data is used
    void run(const Value& program);
    ~VM();
    inline void append(const Value& data, bool clone = true) {
#ifndef USE_ARDUINO_ARRAY
        stack[top] = data;
        stack[top++].copyBeforeModification = clone;
#else
        stack.append(data, clone);
#endif
    }

    inline Value pop() {
#ifndef USE_ARDUINO_ARRAY
        return stack[--top];
#else
        return stack.pop();
#endif
    }
};
#endif // OPCODES_H