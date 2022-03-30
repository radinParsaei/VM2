#include "VM.h"

VM::VM() {
#ifndef USE_DOUBLE
#ifndef USE_BIG_NUMBER
  mpf_set_default_prec(1024);
#else
  BigNumber::begin(10);
#endif
#endif
#ifndef USE_ARDUINO_ARRAY
    valuesToFree.reserve(1000);
#endif
}

void VM::_recorded_program_to_unsigned_long_array(unsigned long array[], const Value& prog, const int& progLength, const bool clone_) {
    bool isData = false;
    for (int i = 0; i < progLength; i++) {
        const Value& item = prog[i];
        if (!isData) {
            array[i] = (int) item;
            if (NEEDS_PARAMETER(array[i])) {
                isData = true;
            }
        } else {
#ifndef USE_ARDUINO_ARRAY
            if (clone_) {
                array[i] = (unsigned long) new Value(item);
                valuesToFree.push_back((Value*) array[i]);
            } else
#endif
            array[i] = (unsigned long) &item;
            isData = false;
        }
    }
}

void VM::_run_program_from_unsigned_long_array(unsigned long array[], const unsigned int& len, void** toFree, unsigned short* toFreeCount) {
    for (int i = 0; i < len; i++) {
        if (NEEDS_PARAMETER(array[i])) {
            run1(array[i], *(Value*) array[i + 1]);
            i++;
        } else {
            run1(array[i]);
            if (array[i] == OPCODE_END && !rec) {
                Value& prog = stackTOP;
                array[i] = (unsigned long) new Value(prog);
                if (toFree) toFree[(*toFreeCount)++] = (void*) array[i];
                if (NEEDS_PARAMETER(array[i - 2])) delete (Value*) array[i - 1];
                array[i - 1] = OPCODE_PUT;
                int l = prog.length();
                for (int j = 0; j < l; j++) {
                    array[i - j - 2] = 0;
                }
            }
        }
        if (flowControlFlags) break;
    }
}

void VM::run(const Value& program) {
    size_t s = program.length();
    for (int i = 0; i < s; i++) {
        if (NEEDS_PARAMETER((int) program[i])) {
            run1((int) program[i], program[i + 1]);
            i++;
        } else {
            run1((int) program[i]);
        }
        if (flowControlFlags) { // if any of flags is set, break (continue, break, return)
            break;
        }
    }
}

bool VM::run1(int opcode, const Value& data) {
    if (skip && skip--) {
        return NEEDS_PARAMETER(opcode);
    }
    if (rec > 0) {
        if (opcode != OPCODE_END || (rec - 1)) stackTOP.append(opcode, false);
        if (opcode == OPCODE_END) {
            rec--;
            return false;
        }
        if (opcode == OPCODE_REC) {
            rec++;
            return false;
        }
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
    case OPCODE_PRINT: {
        const Value& v = stackTOP;
        if (v.getType() == Types::Instance) {
            const Value& toStr = "#toString";
            if (v.containsKey(toStr)) {
                run1(OPCODE_CALLMETHOD, toStr);
            } else {
#ifndef USE_NOSTD_MAP
                std::cout << "{\n";
                std::unordered_map<Value, Value, HashFunction>::iterator it;
                size_t size = std::distance((*v.getData().map).begin(), (*v.getData().map).end()) - 1;
                bool sep = false;
                for (it = (*v.getData().map).begin(); it != (*v.getData().map).end(); it++) {
                    if (it->first.getType() != Types::Text && !(IS_NUM(it->first))) continue;
                    std::cout << '\t' << it->first.toString() << " = " << it->second.toString();
                    if (std::distance((*v.getData().map).begin(), it) != size) {
                        std::cout << "\n";
                        sep = true;
                    } else {
                        sep = false;
                    }
                }
                if (!sep) std::cout << '\n';
                std::cout << "}";
#else
                Serial.println("{");
                bool sep = false;
                for (int i = 0; i < v.getData().map->size(); i++) {
                    if ((*v.getData().map)[i].key->getType() != Types::Text && (*v.getData().map)[i].key->getType() != Types::Number) continue;
                    Serial.print("    ");
                    Serial.println((*v.getData().map)[i].key->toString() + " = " + (*v.getData().map)[i].value->toString());
                    if (i != v.getData().map->size() - 1) {
                        Serial.println();
                        sep = true;
                    } else {
                        sep = false;
                    }
                }
                if (!sep) Serial.println();
                Serial.print("}");
#endif
                _POP_
                break;
            }
        }
#if __has_include("Arduino.h")
        Serial.print(pop().toString());
#else
        std::cout << pop().toString();
#endif
        break;
    }
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
        bool clone = !(v.getType() == Types::Array || v.getType() == Types::Map || v.getType() == Types::Instance);
        append(v, clone);
        return true;
    }
    case OPCODE_INCREASE: {
        mem.get(data) += pop();
        return true;
    }
    case OPCODE_DECREASE: {
        mem.get(data) -= pop();
        return true;
    }
    case OPCODE_INPLACE_MUL: {
        mem.get(data) *= pop();
        return true;
    }
    case OPCODE_INPLACE_DIV: {
        mem.get(data) /= pop();
        return true;
    }
    case OPCODE_INPLACE_MOD: {
        mem.get(data) %= pop();
        return true;
    }
    case OPCODE_INPLACE_POW: {
        mem.get(data).pow(pop());
        return true;
    }
    case OPCODE_INPLACE_AND: {
        mem.get(data) &= pop();
        return true;
    }
    case OPCODE_INPLACE_OR: {
        mem.get(data) |= pop();
        return true;
    }
    case OPCODE_INPLACE_LSHIFT: {
        mem.get(data) <<= pop();
        return true;
    }
    case OPCODE_INPLACE_RSHIFT: {
        mem.get(data) >>= pop();
        return true;
    }
    case OPCODE_INPLACE_XOR: {
        mem.get(data) ^= pop();
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
        bool clone = !(v.getType() == Types::Array || v.getType() == Types::Map || v.getType() == Types::Instance);
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
    case OPCODE_SKIPIFN: {
        const Value& v = pop();
        if (!v) skip = (unsigned short) ((int) data);
        return true;
    }
    case OPCODE_SKIP: {
        skip = (unsigned short) ((int) data);
        return true;
    }
    case OPCODE_WHILET: {
        const Value& prog = pop();
        int progLength = prog.length();
        unsigned long progOpcodes[progLength];
        _recorded_program_to_unsigned_long_array(progOpcodes, prog, progLength);
        void** toFree = new void*[progLength / 2];
        unsigned short toFreeCount = 0;
        while (true) {
            _run_program_from_unsigned_long_array(progOpcodes, progLength, toFree, &toFreeCount);
            if (flowControlFlags & FLOW_CONTROL_CONTINUE) {
                flowControlFlags &= ~FLOW_CONTROL_CONTINUE;
            }
            if (flowControlFlags & FLOW_CONTROL_BREAK) {
                flowControlFlags &= ~FLOW_CONTROL_BREAK;
                goto exit;
            }
            if (flowControlFlags) goto exit;
        }
        exit:
        while (toFreeCount) {
            toFreeCount--;
            delete (Value*) toFree[toFreeCount];
        }
        delete[] toFree;
        break;
    }
    case OPCODE_WHILE: {
        const Value& cond = pop();
        const Value& prog = pop();
        int condLength = cond.length();
        int progLength = prog.length();
        unsigned long condOpcodes[condLength];
        unsigned long progOpcodes[progLength];
        _recorded_program_to_unsigned_long_array(condOpcodes, cond, condLength);
        _recorded_program_to_unsigned_long_array(progOpcodes, prog, progLength);

        unsigned short freeCountCond = 0, freeCountProg = 0;

        void** toFreeCond = new void*[progLength / 2];

        _run_program_from_unsigned_long_array(condOpcodes, condLength, toFreeCond, &freeCountCond);

        void** toFree = new void*[progLength / 2];

        while (pop()) {
            _run_program_from_unsigned_long_array(progOpcodes, progLength, toFree, &freeCountProg);
            if (flowControlFlags & FLOW_CONTROL_CONTINUE) {
                flowControlFlags &= ~FLOW_CONTROL_CONTINUE;
            }
            if (flowControlFlags & FLOW_CONTROL_BREAK) {
                flowControlFlags &= ~FLOW_CONTROL_BREAK;
                goto _exit;
            }
            if (flowControlFlags) goto _exit;
            _run_program_from_unsigned_long_array(condOpcodes, condLength);
        }
        _exit:
        while (freeCountCond) {
            freeCountCond--;
            delete (Value*) toFreeCond[freeCountCond];
        }
        while (freeCountProg) {
            freeCountProg--;
            delete (Value*) toFree[freeCountProg];
        }
        delete[] toFree;
        delete[] toFreeCond;
        break;
    }
    case OPCODE_MKFUNC: {
        const Value& paramCount = pop();
        const Value& body = pop();
#ifndef USE_ARDUINO_ARRAY
        size_t length = body.length();
        unsigned long* opcodes = new unsigned long[length + 2];
        opcodes[0] = (int) paramCount;
        opcodes[1] = length;
        _recorded_program_to_unsigned_long_array(opcodes + 2, body, length, true);
        functions[data] = opcodes;
        lastFunc = opcodes;
#else
        body.append(paramCount, false);
        functions.put(data, body);
#endif
        return true;
    }
    case OPCODE_CALLFUNC: {
#ifndef USE_ARDUINO_ARRAY
        unsigned long* prog;
        if (data.getType() == Types::FuncPtr) {
            prog = (unsigned long*) (unsigned long) (double) data;
        } else {
            prog = functions[data];
        }
        short l = prog[0];
        for (short i = 0; i < l; i++) {
            params.append(pop(), false);
        }
        unsigned long length = prog[1];
        void** toFree = new void*[length / 2];
        unsigned short toFreeCount = 0;
        _run_program_from_unsigned_long_array(prog + 2, length, toFree, &toFreeCount);
        if (flowControlFlags & FLOW_CONTROL_RETURN) {
            flowControlFlags &= ~FLOW_CONTROL_RETURN;
        }
        while (toFreeCount) {
            toFreeCount--;
            valuesToFree.push_back((Value*) toFree[toFreeCount]);
        }
        delete[] toFree;
#else
        const Value prog;
        if (data.getType() == Types::FuncPtr) {
            prog = data;
        } else {
            prog = functions[data];
        }
        short l = (int) prog.pop();
        for (int i = 0; i < l; i++) {
            params.append(pop(), false);
        }
        run(prog);
        if (flowControlFlags & FLOW_CONTROL_RETURN) {
            flowControlFlags &= ~FLOW_CONTROL_RETURN;
        }
#endif
        if (l) params.remove(params.length() - l, params.length());
        return true;
    }
    case OPCODE_GETPARAM: {
        append(params[params.length() - (long) data], false);
        return true;
    }
    case OPCODE_RETURN:
        flowControlFlags |= FLOW_CONTROL_RETURN;
        break;
    case OPCODE_CONTINUE:
        flowControlFlags |= FLOW_CONTROL_CONTINUE;
        break;
    case OPCODE_BREAK:
        flowControlFlags |= FLOW_CONTROL_BREAK;
        break;
    case OPCODE_CREATE_CLASS: {
        Value v = pop();
        v.copyBeforeModification = false;
        v.setType(Types::Instance);
        if (v.containsKey(Types::False)) {
            const Value& parents = v.get(Types::False);
            for (char i = 0; i < parents.length(); i++) {
                for (short j = 0; j < classes.get(parents[i]).length(); j++) {
                    const Value& key = classes.get(parents[i]).getKeyAt(j);
                    if (!v.containsKey(key)) v.set(key, classes.get(parents[i]).getValueAt(j));
                }
            }
        }
        v.put(Types::Null, data);
        classes.put(data, v);
        return true;
    }
    case OPCODE_CREATE_INSTANCE: {
        append(classes[data], true);
        if (stackTOP.containsKey("<init>")) {
            run1(OPCODE_CALLMETHOD, "<init>");
        }
        return true;
    }
    case OPCODE_CALLFUNCFROMINS: {
        const Value& tmp = self;
        self = pop();
        run1(OPCODE_CALLFUNC, data);
        self = tmp;
        return true;
    }
    case OPCODE_CALLMETHOD: {
        const Value& tmp = self;
        if (data.startsWith("<"))
            self = stackTOP;
        else
            self = pop();
        run1(OPCODE_CALLFUNC, self.get(data));
        self = tmp;
        return true;
    }
    case OPCODE_THIS: {
        append(self, false);
        break;
    }
    case OPCODE_GETPTRTOLASTFUNC: {
#ifdef USE_ARDUINO_ARRAY
        Value v = functions.getValueAt(functions.length() - 1);
        v.setType(Types::FuncPtr);
        append(v, true);
#else
        Value v = (double) (unsigned long) lastFunc;
        v.setType(Types::FuncPtr);
        append(v, true);
#endif
        break;
    } case OPCODE_IS: {
        if (IS_NUM(data)) {
            char type = (int) data;
            char type2 = (char) stackTOP.getType();
            if (type == 3) {
                append(type2 == (char) Types::True || type2 == (char) Types::False, false);
            } else if (type == 4) {
                append(type == (char) Types::Number || type == (char) Types::BigNumber, false);
            } else {
                append(type == type2, false);
            }
        } else if (stackTOP.getType() == Types::Instance) {
            if (stackTOP.getType() == Types::Instance) {
                if (data == stackTOP.get(Types::Null)) {
                    append(true, false);
                } else {
                    bool isInstanceOf = false;
                    if (stackTOP.containsKey(Types::False)) {
                        const Value& parents = stackTOP.get(Types::False);
                        for (char i = 0; i < parents.length(); i++) {
                            if (parents[i] == data) {
                                isInstanceOf = true;
                                break;
                            }
                        }
                    }
                    append(isInstanceOf, false);
                }
            }
        } else {
            append(false, false);
        }
        break;
    }
    }
    return false;
}

VM::~VM() {
#ifndef USE_ARDUINO_ARRAY
    for (Value* v : valuesToFree) {
        delete v;
    }
    for (auto& i : functions) {
        delete[] i.second;
    }
#endif
}