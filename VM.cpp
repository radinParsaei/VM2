#include "VM.h"

#ifdef ARDUINO_HARDWARE
#include <Arduino.h>
#endif

VM::VM() {
#ifndef USE_DOUBLE
#ifndef USE_BIG_NUMBER
  mpf_set_default_prec(1024);
#else
  BigNumber::begin(10);
#endif
#endif
#if !defined(USE_ARDUINO_ARRAY) && !defined(ESP32) && !defined(ESP8266) && !defined(MICROBIT) && !defined(ARDUINO_ARCH_SAM)
    valuesToFree.reserve(1000);
#endif
}

bool bundledLibCall(const Value& fileName, const Value& funcName, Value* args, int argc, Value*& res);

void VM::_recorded_program_to_unsigned_long_array(unsigned long array[], const Value& prog, const int progLength, const bool clone_, void** toFree, unsigned short* toFreeCount) {
    bool isData = false;
    unsigned char rec = 0;
    for (int i = 0; i < progLength; i++) {
        const Value& item = prog[i];
        if (!isData) {
            int intVersion = item;
            array[i] = intVersion;
            if (i < progLength - 1 && prog[i + 1].getType() != Types::Ptr && (intVersion == OPCODE_GETVAR || intVersion == OPCODE_SETVAR || intVersion == OPCODE_INCREASE || intVersion == OPCODE_DECREASE || intVersion == OPCODE_INPLACE_MUL || intVersion == OPCODE_INPLACE_DIV || intVersion == OPCODE_INPLACE_MOD || intVersion == OPCODE_INPLACE_POW || intVersion == OPCODE_INPLACE_AND || intVersion == OPCODE_INPLACE_OR || intVersion == OPCODE_INPLACE_LSHIFT || intVersion == OPCODE_INPLACE_RSHIFT || intVersion == OPCODE_INPLACE_XOR)) {
                i++;
                Value* ptr = new Value((long) &mem.get(prog[i]));
                ptr->setType(Types::Ptr);
                array[i] = (unsigned long) ptr;
#if !defined(USE_ARDUINO_ARRAY) && !defined(ESP32) && !defined(ESP8266) && !defined(MICROBIT) && !defined(ARDUINO_ARCH_SAM)
                if (!clone_) valuesToFree.push_back(ptr);
                else if (toFree) toFree[(*toFreeCount)++] = (void*) array[i];
#endif
            } else if (NEEDS_PARAMETER(intVersion)) {
                isData = true;
            } else if (intVersion == OPCODE_REC) {
                rec++;
            } else if (intVersion == OPCODE_END) {
                rec--;
            }
        } else {
#if !defined(USE_ARDUINO_ARRAY) && !defined(ESP32) && !defined(ESP8266) && !defined(MICROBIT) && !defined(ARDUINO_ARCH_SAM)
            if (clone_) {
                array[i] = (unsigned long) new Value(item);
                ((Value*) array[i])->clone();
                if (!rec) valuesToFree.push_back((Value*) array[i]);
            } else
#endif
            array[i] = (unsigned long) &item;
            isData = false;
        }
    }
}

void VM::_run_program_from_unsigned_long_array(unsigned long array[], const unsigned int len, void** toFree, unsigned short* toFreeCount) {
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
                array[i - 1] = OPCODE_PUT;
                int l = prog.length();
                for (int j = 0; j < l; j++) {
                    array[i - j - 2] = 0;
                }
            }
        }
        if (flowControlFlags) break;
        if (skip) {
            i += skip;
            skip = 0;
        }
    }
}

Value& getFromClass(Value instance, Value item) {
    while (instance != Types::Null) {
        if (instance.containsKey(item)) {
            return instance[item];
        }
        instance = instance.get(Types::True);
    }
    return __NULL__;
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
        if (data.getType() == Types::Ptr) {
            Value& v = *((Value*) (unsigned long) (long) data);
            v = pop();
            return true;
        }
        mem.put(data, pop());
        return true;
    }
    case OPCODE_GETVAR: {
        if (data.getType() == Types::Ptr) {
            Value& v = *((Value*) (unsigned long) (long) data);
            bool clone = !(v.getType() == Types::Array || v.getType() == Types::Map || v.getType() == Types::Instance);
            append(v, clone);
            return true;
        }
        const Value& v = mem.get(data);
        bool clone = !(v.getType() == Types::Array || v.getType() == Types::Map || v.getType() == Types::Instance);
        append(v, clone);
        return true;
    }
    case OPCODE_DELVAR: {
        mem.remove(data);
        return true;
    }
    case OPCODE_INCREASE: {
        if (data.getType() == Types::Ptr) {
            Value& v = *((Value*) (unsigned long) (long) data);
            v += pop();
            return true;
        }
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
        if (arr.getType() == Types::Instance) {
            const Value& v = getFromClass(arr, i);
            append(v, false);
            break;
        }
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
        if (arr.getType() == Types::Instance) {
            Value* res;
            Value instance = arr;
            while (instance != Types::Null) {
                if (instance.containsKey(i)) {
                    res = &instance;
                    break;
                }
                instance = instance.get(Types::True);
            }
            instance.copyBeforeModification = false;
            instance[i] = v;
            break;
        }
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
        if (!v) skip = (short) ((int) data);
        return true;
    }
    case OPCODE_SKIPIF: {
        const Value& v = pop();
        if (v) skip = (short) ((int) data);
        return true;
    }
    case OPCODE_SKIP: {
        skip = (short) ((int) data);
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
        Value prog = pop();
        int condLength = cond.length();
        int progLength = prog.length();
        unsigned long condOpcodes[condLength];
        unsigned long progOpcodes[progLength];

        unsigned short freeCountCond = 0, freeCountProg = 0;

        void** toFreeCond = new void*[progLength / 2];
        void** toFree = new void*[progLength / 2];

        _recorded_program_to_unsigned_long_array(condOpcodes, cond, condLength, false, toFreeCond, &freeCountCond);
        _recorded_program_to_unsigned_long_array(progOpcodes, prog, progLength, false, toFree, &freeCountProg);

        _run_program_from_unsigned_long_array(condOpcodes, condLength, toFreeCond, &freeCountCond);

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
        Value body = pop();
#if !defined(USE_ARDUINO_ARRAY) && !defined(ESP32) && !defined(ESP8266) && !defined(MICROBIT) && !defined(ARDUINO_ARCH_SAM)
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
        char dataType = (char) data.getType();
        if (dataType == (char) Types::Null || dataType == (char) Types::False) { // null -> print, false -> println
            const Value& v = stackTOP;
            if (v.getType() == Types::Instance) {
                const Value& toStr = "#toString";
                const Value& tmp = self;
                self = pop();
                Value instance = self;
                Value* func;
                while (self != Types::Null) {
                    if (self.containsKey(toStr)) {
                        func = &self[toStr];
                        break;
                    }
                    self = self.get(Types::True);
                }
                if (func) {
                    run1(OPCODE_CALLFUNC, *func);
                    self = tmp;
                } else {
                    self = tmp;
#ifndef USE_NOSTD_MAP
#ifdef ARDUINO_HARDWARE
                    Serial.println("{\n");
                    std::unordered_map<Value, Value, HashFunction>::iterator it;
                    size_t size = std::distance((*v.getData().map).begin(), (*v.getData().map).end()) - 1;
                    bool sep = false;
                    for (it = (*v.getData().map).begin(); it != (*v.getData().map).end(); it++) {
                        if (it->first.getType() != Types::Text && !(IS_NUM(it->first))) continue;
                        Serial.print('\t');
                        Serial.print(it->first.toString().c_str());
                        Serial.print(" = ");
                        Serial.print(it->second.toString().c_str());
                        if (std::distance((*v.getData().map).begin(), it) != size) {
                            Serial.println();
                            sep = true;
                        } else {
                            sep = false;
                        }
                    }
                    if (!sep) Serial.println();
                    Serial.print("}");
                    if (data == Types::False) {
                        Serial.println();
                    }
#else
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
                    if (dataType == (char) Types::False) {
                        std::cout << std::endl;
                    }
#endif
#else
                    Serial.println("{");
                    bool sep = false;
                    for (int i = 0; i < v.getData().map->size(); i++) {
                        if ((*v.getData().map)[i].key->getType() != Types::Text && (*v.getData().map)[i].key->getType() != Types::Number) continue;
                        Serial.print("    ");
                        Serial.println((*v.getData().map)[i].key->toString() + " = " + (*v.getData().map)[i].value->toString().c_str());
                        if (i != v.getData().map->size() - 1) {
                            Serial.println();
                            sep = true;
                        } else {
                            sep = false;
                        }
                    }
                    if (!sep) Serial.println();
                    Serial.print("}");
                    if (dataType == (char) Types::False) {
                        Serial.println();
                    }
#endif
                    _POP_
                    break;
                }
            }
#ifdef ARDUINO_HARDWARE
            Serial.print(pop().toString().c_str());
            if (dataType == (char) Types::False) {
                Serial.println();
            }
#else
            std::cout << pop().toString();
            if (dataType == (char) Types::False) {
                std::cout << '\n';
            }
#endif
            break;
        } else if (dataType == (char) Types::True) { // -> input
            TEXT a;
#ifndef ARDUINO_HARDWARE
            std::getline(std::cin, a);
#else
            a = Serial.readString().c_str();
#endif
            append(a, false);
            break;
        } else if (data == Value(-1)) { // -> flush
#ifndef ARDUINO_HARDWARE
            std::cout << std::flush;
#else
            Serial.flush();
#endif
            break;
        }
#if !defined(USE_ARDUINO_ARRAY) && !defined(ESP32) && !defined(ESP8266) && !defined(MICROBIT) && !defined(ARDUINO_ARCH_SAM)
        unsigned long* prog;
        if (dataType == (char) Types::FuncPtr) {
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
        Value prog;
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
            v.set(Types::True, Types::Map);
            const Value& parent = v.get(Types::False);
            for (short i = 0; i < classes.get(parent).length(); i++) {
                v.get(Types::True).set(classes.get(parent).getKeyAt(i), classes.get(parent).getValueAt(i));
            }
        }
        v.put(Types::Null, data);
        classes.put(data, v);
        return true;
    }
    case OPCODE_CREATE_INSTANCE: {
        append(classes[data], true);
        run1(OPCODE_CALLMETHOD, "<init>");
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
        self = pop();
        Value instance = self;
        Value* func;
        while (self != Types::Null) {
            if (self.containsKey(data)) {
                func = &self[data];
                break;
            }
            self = self.get(Types::True);
        }
        if (!func) goto method_call_exit;
        run1(OPCODE_CALLFUNC, *func);
        method_call_exit:
        if (data.startsWith("<")) append(instance, false);
        self = tmp;
        return true;
    }
    case OPCODE_THIS: {
        append(self, false);
        break;
    }
    case OPCODE_GETPTRTOLASTFUNC: {
#if defined(USE_ARDUINO_ARRAY) || defined(ESP32) || defined(ESP8266) || defined(MICROBIT) || defined(ARDUINO_ARCH_SAM)
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
    case OPCODE_DLCALL: {
        const Value& functionName = pop();
        const Value& fileName = pop();
        int argc = data;
        Value* v = new Value[argc];
        for (int i = 0; i < argc; i++) {
            v[i] = &stack[_STACK_LEN_ - i];
        }
        Value* res;
#ifndef ARDUINO_HARDWARE
        if (!bundledLibCall(fileName, functionName, v, argc, res)) { // file name has to start with : to point to a statically linked library
            dlfunc fn;
            void*& lib = libs[fileName.toString()];
            if (lib == 0) {
                lib = dlopen(fileName.toString().c_str(), RTLD_LAZY);
            }
            if (!lib) {
                std::cerr << "opening library \"" << fileName.toString() << "\" failed\n";
                std::cerr << dlerror();
                break;
            }
            dlerror(); //clear errors
            fn = ((dlfunc) dlsym(lib, functionName.toString().c_str()));
            res = fn(v, argc);
        }
#else
        bundledLibCall(fileName, functionName, v, argc, res);
#endif
        if (res != 0) {
            append(Value(res->getData(), res->getType(), res->useCount), false);
            delete res;
        }
        for (int i = 0; i < argc; i++) {
            _POP_
        }
        delete[] v;
        return true;
    }
    }
    return false;
}

VM::~VM() {
#if !defined(USE_ARDUINO_ARRAY) && !defined(ESP32) && !defined(ESP8266) && !defined(MICROBIT) && !defined(ARDUINO_ARCH_SAM)
    for (Value* v : valuesToFree) {
        delete v;
    }
    for (auto& i : functions) {
        delete[] i.second;
    }
#endif
#ifndef ARDUINO_HARDWARE
    for (auto& v : libs) {
        dlclose(v.second);
    }
#endif
}
