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
        stackTOP.append(opcode, false);
        if (NEEDS_PARAMETER(opcode)) {
            stackTOP.append(data, false);
            return true;
        }
        return false;
    }
    switch (opcode) {
    case OPCODE_PUT: // append data to stack
        append(data, false);
        return true;
    case OPCODE_PRINT:
#if __has_include("Arduino.h")
        Serial.print(pop().toString());
#else
        std::cout << pop().toString();
#endif
        break;
    case OPCODE_ADD: {
        const Value& x = pop();
        stackTOP += x;
        break;
    }
    case OPCODE_POP: {
        _POP_
        break;
    }
    case OPCODE_SUB: {
        const Value& x = pop();
        stackTOP -= x;
        break;
    }
    case OPCODE_MUL: {
        const Value& x = pop();
        stackTOP *= x;
        break;
    }
    case OPCODE_DIV: {
        const Value& x = pop();
        stackTOP /= x;
        break;
    }
    case OPCODE_MOD: {
        const Value& x = pop();
        stackTOP %= x;
        break;
    }
    case OPCODE_POW: {
        const Value& x = pop();
        stackTOP.pow(x);
        break;
    }
    case OPCODE_EQ: {
        const Value& x = pop();
        stackTOP = stackTOP == x;
        break;
    }
    case OPCODE_NEQ: {
        const Value& x = pop();
        stackTOP = stackTOP != x;
        break;
    }
    case OPCODE_GT: {
        const Value& x = pop();
        stackTOP = stackTOP > x;
        break;
    }
    case OPCODE_GE: {
        const Value& x = pop();
        stackTOP = stackTOP >= x;
        break;
    }
    case OPCODE_LT: {
        const Value& x = pop();
        stackTOP = stackTOP < x;
        break;
    }
    case OPCODE_LE: {
        const Value& x = pop();
        stackTOP = stackTOP <= x;
        break;
    }
    case OPCODE_LEQ: {
        const Value& x = pop();
        stackTOP = stackTOP.looksEqual(x);
        break;
    }
    case OPCODE_AND: {
        const Value& x = pop();
        stackTOP = stackTOP && x;
        break;
    }
    case OPCODE_OR: {
        const Value& x = pop();
        stackTOP = stackTOP || x;
        break;
    }
    case OPCODE_B_OR: {
        const Value& x = pop();
        stackTOP |= x;
        break;
    }
    case OPCODE_B_AND: {
        const Value& x = pop();
        stackTOP &= x;
        break;
    }
    case OPCODE_B_NOT: {
        stackTOP = ~stackTOP;
        break;
    }
    case OPCODE_NEGATE: {
        stackTOP = -stackTOP;
        break;
    }
    case OPCODE_NOT: {
        stackTOP = !stackTOP;
        break;
    }
    case OPCODE_LSHIFT: {
        const Value& x = pop();
        stackTOP <<= x;
        break;
    }
    case OPCODE_RSHIFT: {
        const Value& x = pop();
        stackTOP >>= x;
        break;
    }
    case OPCODE_XOR: {
        const Value& x = pop();
        stackTOP ^= x;
        break;
    }
    case OPCODE_SETVAR: {
        mem.put(data, pop());
        return true;
    }
    case OPCODE_GETVAR: {
        const Value& v = mem.get(data);
        bool clone = !(v.getType() == Types::Array || v.getType() == Types::Map);
        append(v, clone);
        return true;
    }
    case OPCODE_INCREASE: {
        mem.get(data) += pop();
        return true;
    }
    case OPCODE_CREATE_ARR: {
        Value arr(Types::Array);
        long l = data;
        for (int i = 0; i < l; i++) {
            arr.append(pop(), false);
        }
        append(arr, false);
        return true;
    }
    case OPCODE_CREATE_MAP: {
        Value map(Types::Map);
        long l = data;
        for (int i = 0; i < l; i++) {
            const Value& v = pop(); // value
            const Value& k = pop(); // key
            map.set(k, v);
        }
        append(map, false);
        return true;
    }
    case OPCODE_GET: {
        const Value& i = pop();
        const Value& arr = pop();
        const Value& v = arr[i];
        bool clone = !(v.getType() == Types::Array || v.getType() == Types::Map);
        append(v, clone);
        break;
    }
    case OPCODE_SET: {
        const Value& v = pop(); // value
        const Value& i = pop(); // index
        // func test() {
        //     print(1)
        //     return 0
        // }
        // func test1() {
        //     print(2)
        //     return 0
        // }
        // a = []
        // a[test()] = test1()
        //
        // Result:
        //   1
        //   2
        Value& arr = stackTOP;
        arr.copyBeforeModification = false;
        arr[i] = v;
        break;
    }
    case OPCODE_REC: {
        if (rec++ == 0) { // rec was 0
            append(Types::Array, false);
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
            if (NEEDS_PARAMETER(condOpcodes[i])) {
                run1(condOpcodes[i], *(Value*) condOpcodes[i + 1]);
                i++;
            } else {
                run1(condOpcodes[i]);
            }
        }

        while (pop()) {
            for (int i = 0; i < progLength; i++) {
                if (NEEDS_PARAMETER(progOpcodes[i])) {
                    run1(progOpcodes[i], *(Value*) progOpcodes[i + 1]);
                    i++;
                } else {
                    run1(progOpcodes[i]);
                }
            }
            for (int i = 0; i < condLength; i++) {
                if (NEEDS_PARAMETER(condOpcodes[i])) {
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