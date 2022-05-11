#ifndef VM_H
#define VM_H
#include "opcodes.h"

#if defined(ARDUINO_ARCH_SAM) || defined(ESP8266) || defined(ESP32)
#define ARDUINO_HARDWARE
#else
#if __has_include("Arduino.h")
#define ARDUINO_HARDWARE
#endif
#endif

#ifdef ARDUINO_HARDWARE
#undef min
#undef max
#endif

#ifdef __AVR__
#define USE_DOUBLE
// #define USE_BIG_NUMBER
#define USE_ARDUINO_STRING
#define USE_ARDUINO_ARRAY
#define USE_NOSTD_MAP
#define MAX_FIXED_ARRAY_SIZE 10
#elif defined(ARDUINO_HARDWARE)
#define USE_BIG_NUMBER
#else
#define VECTOR_RESERVED_SIZE 1000
#endif
#ifndef ARDUINO_HARDWARE
#include <iostream>
#include <array>
#include <map>
#endif

// define instance type for value
#define __ADDITIONAL_TYPES__ Instance, FuncPtr
#define TREAT_AS_MAP(x) || x == Types::Instance
#ifdef USE_ARDUINO_ARRAY
#define TREAT_AS_ARRAY(x) || x == Types::FuncPtr // on arduino (with arduino array) it contains the recorded program
#else
#define TREAT_AS_NUMBER(x) || x == Types::FuncPtr // elsewhere it will save pointer to function
#endif

#include "value.h"

#if !__has_include("Arduino.h") && !defined(ARDUINO_HARDWARE)
#include <dlfcn.h>
#else
#define STATIC_BUILD // let libraries know that they are bundling with the VM
#endif

#define FLOW_CONTROL_CONTINUE       0b00000001
#define FLOW_CONTROL_BREAK          0b00000010
#define FLOW_CONTROL_RETURN         0b00000100

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
typedef Value* (__stdcall *dlfunc)(Value*, int);
#else
typedef Value* (*dlfunc)(Value*, int);
#endif

#define NEEDS_PARAMETER(opcode) opcode == OPCODE_PUT || opcode == OPCODE_GETVAR || opcode == OPCODE_SETVAR || opcode == OPCODE_CREATE_ARR || opcode == OPCODE_CREATE_MAP || opcode == OPCODE_INCREASE || opcode == OPCODE_MKFUNC || opcode == OPCODE_CALLFUNC || opcode == OPCODE_GETPARAM || opcode == OPCODE_SKIPIFN || opcode == OPCODE_SKIP || opcode == OPCODE_CREATE_CLASS || opcode == OPCODE_CREATE_INSTANCE || opcode == OPCODE_CALLFUNCFROMINS || opcode == OPCODE_CALLMETHOD || opcode == OPCODE_IS || opcode == OPCODE_DECREASE || opcode == OPCODE_INPLACE_MUL || opcode == OPCODE_INPLACE_DIV || opcode == OPCODE_INPLACE_MOD || opcode == OPCODE_INPLACE_POW || opcode == OPCODE_INPLACE_AND || opcode == OPCODE_INPLACE_OR || opcode == OPCODE_INPLACE_LSHIFT || opcode == OPCODE_INPLACE_RSHIFT || opcode == OPCODE_INPLACE_XOR || opcode == OPCODE_DLCALL

class VM {
private:
#if !__has_include("Arduino.h")
    std::unordered_map<std::string, void*> libs;
#endif
    void _run_program_from_unsigned_long_array(unsigned long array[], const unsigned int& len, void** toFree = 0, unsigned short* toFreeCount = 0);
    void _recorded_program_to_unsigned_long_array(unsigned long array[], const Value& prog, const int& progLength, const bool clone_ = false);
#if !defined(USE_ARDUINO_ARRAY) && !defined(ESP32) && !defined(ESP8266) && !defined(MICROBIT) && !defined(ARDUINO_ARCH_SAM)
    std::unordered_map<Value, unsigned long*, HashFunction> functions;
    unsigned long* lastFunc; // used in OPCODE_GETPTRTOLASTFUNC (useful while creating classes)
    std::vector<Value*> valuesToFree;
    std::array<Value, 255> stack;
    char top = 0;
#define stackTOP stack[top - 1]
#define _POP_ top --;
#define _STACK_LEN_ top
#else
    Value functions = Types::Map;
    Value stack = Types::Array;
#define stackTOP stack[stack.length() - 1]
#define _POP_ stack._pop();
#define _STACK_LEN_ stack.length()
#endif
    Value mem = Types::Map;
    unsigned char rec = 0;
    Value params = Types::Array; // holds function parameters
    char flowControlFlags = 0;
    unsigned short skip = 0;
    Value classes = Types::Map;
    Value self;
public:
    VM();
    bool run1(int opcode, const Value& data = Types::Null); // returns true if the data is used
    void run(const Value& program);
    ~VM();
    inline void append(const Value& data, bool clone) {
#if !defined(USE_ARDUINO_ARRAY) && !defined(ESP32) && !defined(ESP8266) && !defined(MICROBIT) && !defined(ARDUINO_ARCH_SAM)
        stack[top] = data;
        stack[top++].copyBeforeModification = clone;
#else
        stack.append(data, clone);
#endif
    }

    inline Value pop() {
#if !defined(USE_ARDUINO_ARRAY) && !defined(ESP32) && !defined(ESP8266) && !defined(MICROBIT) && !defined(ARDUINO_ARCH_SAM)
        return stack[--top];
#else
        return stack.pop();
#endif
    }
};
#endif // OPCODES_H