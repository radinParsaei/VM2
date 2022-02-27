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
        if (NEEDS_PARAMETER(opcode)) {
            stack[stack.length() - 1].append(data);
            return true;
        }
        return false;
    }
    switch (opcode) {
    case OPCODE_PUT: // append data to stack
        stack.append(data);
        return true;
    case OPCODE_PRINT:
#if __has_include("Arduino.h")
        Serial.print(stack.pop().toString());
#else
        std::cout << stack.pop().toString();
#endif
        break;
    case OPCODE_ADD: {
        const Value& x = stack.pop();
        stack[stack.length() - 1] += x;
        break;
    }
    case OPCODE_SUB: {
        const Value& x = stack.pop();
        stack[stack.length() - 1] -= x;
        break;
    }
    case OPCODE_MUL: {
        const Value& x = stack.pop();
        stack[stack.length() - 1] *= x;
        break;
    }
    case OPCODE_DIV: {
        const Value& x = stack.pop();
        stack[stack.length() - 1] /= x;
        break;
    }
    case OPCODE_MOD: {
        const Value& x = stack.pop();
        stack[stack.length() - 1] %= x;
        break;
    }
    case OPCODE_POW: {
        const Value& x = stack.pop();
        stack[stack.length() - 1].pow(x);
        break;
    }
    case OPCODE_NEQ: {
        const Value& x = stack.pop();
        stack[stack.length() - 1] = stack[stack.length() - 1] != x;
        break;
    }
    case OPCODE_SETVAR: {
        mem.put(data, stack.pop());
        return true;
    }
    case OPCODE_GETVAR: {
        stack.append(mem.get(data));
        return true;
    }
    case OPCODE_REC: {
        if (rec++ == 0) { // rec was 0
            stack.unsafeAppend(Types::Array);
        }
        break;
    }
    case OPCODE_IF: {
        if (stack.pop())
            run(stack.pop());
        break;
    }
    case OPCODE_WHILE: {
        const Value& cond = stack.pop();
        const Value& prog = stack.pop();
        int condLength = cond.length();
        int progLength = prog.length();
        unsigned long condOpcodes[condLength];
        unsigned long progOpcodes[progLength];
        bool isData = false;
        for (int i = 0; i < condLength; i++) {
            const Value& item = cond[i];
            if (!isData) {
                condOpcodes[i] = (int) item;
                if (NEEDS_PARAMETER(condOpcodes[i])) {
                    isData = true;
                }
            } else {
                condOpcodes[i] = (unsigned long) &item;
                isData = false;
            }
        }
        for (int i = 0; i < progLength; i++) {
            const Value& item = prog[i];
            if (!isData) {
                progOpcodes[i] = (int) item;
                if (NEEDS_PARAMETER(progOpcodes[i])) {
                    isData = true;
                }
            } else {
                progOpcodes[i] = (unsigned long) &item;
                isData = false;
            }
        }

        for (int i = 0; i < condLength; i++) {
            if (condOpcodes[i] == OPCODE_PUT || condOpcodes[i] == OPCODE_GETVAR || condOpcodes[i] == OPCODE_SETVAR) {
                run1(condOpcodes[i], *(Value*) condOpcodes[i + 1]);
                i++;
            } else {
                run1(condOpcodes[i]);
            }
        }

        while (stack.pop()) {
            for (int i = 0; i < progLength; i++) {
                if (progOpcodes[i] == OPCODE_PUT || progOpcodes[i] == OPCODE_GETVAR || progOpcodes[i] == OPCODE_SETVAR) {
                    run1(progOpcodes[i], *(Value*) progOpcodes[i + 1]);
                    i++;
                } else {
                    run1(progOpcodes[i]);
                }
            }
            for (int i = 0; i < condLength; i++) {
                if (condOpcodes[i] == OPCODE_PUT || condOpcodes[i] == OPCODE_GETVAR || condOpcodes[i] == OPCODE_SETVAR) {
                    run1(condOpcodes[i], *(Value*) condOpcodes[i + 1]);
                    i++;
                } else {
                    run1(condOpcodes[i]);
                }
            }
        }
        break;
    }
    }
    return false;
}