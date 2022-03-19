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

#define NEEDS_PARAMETER(opcode) opcode == OPCODE_PUT || opcode == OPCODE_GETVAR || opcode == OPCODE_SETVAR || opcode == OPCODE_CREATE_ARR || opcode == OPCODE_CREATE_MAP || opcode == OPCODE_INCREASE || opcode == OPCODE_MKFUNC || opcode == OPCODE_CALLFUNC || opcode == OPCODE_GETPARAM

class VM {
private:
#ifndef USE_ARDUINO_ARRAY
    std::unordered_map<Value, unsigned long*, HashFunction> functions;
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
    short rec = 0;
    Value params = Types::Array; // holds function parameters
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