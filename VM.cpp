#include "VM.h"

VM::VM() {
#ifndef USE_DOUBLE
#ifndef USE_BIG_NUMBER
  mpf_set_default_prec(1024);
#else
  BigNumber::begin(10);
#endif
#endif
}

inline Value VM::pop() {
    return stack.pop();
}

inline void VM::put(const Value& v) {
    stack.append(v);
}

void VM::run(const Value& program) {
    if (program.getType() == Types::Array) {
        for (int i = 0; i < program.length(); i++) {
            if (run1((int) program[i], i != (program.length() - 1)? program[i + 1] : Types::Null)) i++;
        }
    }
}

bool VM::run1(int opcode, const Value& data) {
    if (opcode == OPCODE_END) {
        rec--;
    }
    if (rec > 0) {
        stack[stack.length() - 1].append(opcode);
        if (opcode == OPCODE_PUT || opcode == OPCODE_GETVAR || opcode == OPCODE_SETVAR) {
            stack[stack.length() - 1].append(data);
            return true;
        }
        return false;
    }
    switch (opcode) {
    case OPCODE_PUT: // append data to stack
        put(data);
        return true;
    case OPCODE_PRINT:
#if __has_include("Arduino.h")
        Serial.print(pop().toString());
#else
        std::cout << pop().toString();
#endif
        break;
    case OPCODE_ADD: {
        Value x = pop();
        stack[stack.length() - 1] += x;
        break;
    }
    case OPCODE_SUB: {
        Value x = pop();
        stack[stack.length() - 1] -= x;
        break;
    }
    case OPCODE_MUL: {
        Value x = pop();
        stack[stack.length() - 1] *= x;
        break;
    }
    case OPCODE_DIV: {
        Value x = pop();
        stack[stack.length() - 1] /= x;
        break;
    }
    case OPCODE_MOD: {
        Value x = pop();
        stack[stack.length() - 1] %= x;
        break;
    }
    case OPCODE_POW: {
        Value x = pop();
        stack[stack.length() - 1].pow(x);
        break;
    }
    case OPCODE_SETVAR: {
        mem.put(data, pop());
        return true;
    }
    case OPCODE_GETVAR: {
        put(mem.get(data));
        return true;
    }
    case OPCODE_REC: {
        if (rec++ == 0) { // rec was 0
            put(Types::Array);
        }
        break;
    }
    case OPCODE_IF: {
        if (pop())
            run(pop());
        break;
    }
    case OPCODE_WHILE: {
        const Value& cond = pop();
        const Value& prog = pop();
        run(cond);
        while (pop()) {
            run(prog);
            run(cond);
        }
        break;
    }
    }
    return false;
}