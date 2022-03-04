#ifndef VM_BINARIES_H
#define VM_BINARIES_H

#include "VM.h"

namespace VM_BINARIES {
  class VMBinStream {
    public:
    virtual bool get(char& data) = 0;
  };

  class VMStringStream : public VMBinStream {
    public:
    const char *data;
    int len;
    int i = 0;
    bool get(char& d) {
      if (i != len) {
        d = data[i++];
        return true;
      }
      return false;
    }
  };

  bool readValue(VMBinStream& in, char& data, Value& program) {
    if (data == 0) {
      program = (Types::Null);
    } else if (data == 1) {
      program = (Types::True);
    } else if (data == 2) {
      program = (Types::False);
    } else if ((data & 0b00000111) == 3 || (data & 0b00000111) == 4) { //small numbers
      bool isneg = (data & 0b00000111) - 3;
      Value n = (data & 0b11111000) >> 3;
      while (data != 0) {
        if (!in.get(data)) break;
        n += (unsigned char)data;
      }
      if (isneg) n = -n;
      program = n;
    } else if (data == 5) {
      TEXT s;
      while (true) {
        if (!in.get(data)) break;
        if (!data) {
          if (!in.get(data)) break;
          if (data) break;
        }
        s += data;
      }
      program = s;
      return true;
    } else if (data == 6) {
      TEXT s;
      while (data != 0) {
        if (!in.get(data)) break;
        s += data;
      }
      program = s.c_str();
      program.toNumber();
    }
    return false;
  }

  void runBinFromStream(VMBinStream& in, VM& vm) {
    char data = 0;
    bool dataAvailable = in.get(data);
    while (dataAvailable) {
      if (NEEDS_PARAMETER(data)) {
        short opcode = data;
        if (!in.get(data)) break;
        Value v;
        bool b = readValue(in, data, v);
        vm.run1(opcode, v);
        if (b) continue;
      } else {
        vm.run1(data);
      }
      dataAvailable = (bool)in.get(data);
    }
  }

  Value parseString(VMBinStream& in) {
    Value program = Types::Array;
    char data = 0;
    bool dataAvailable = (bool)(bool)in.get(data);
    bool tmp = false;
    while (dataAvailable) {
      program.append((int) data);
      if (NEEDS_PARAMETER(data)) {
        if (!in.get(data)) break;
        Value v;
        tmp = readValue(in, data, v);
        program.append(v);
        if (tmp) continue;
      }
      dataAvailable = (bool) in.get(data);
      if (tmp && !dataAvailable) {
        program._pop();
      }
      tmp = false;
    }

    return program;
  }

  void appendData(const Value& data, TEXT& res) {
    switch (data.getType()) {
      case Types::Null:
        res += (char) 0;
        break;
      case Types::True:
        res += (char) 1;
        break;
      case Types::False:
        res += (char) 2;
        break;
      case Types::Number:
      case Types::BigNumber: {
        NUMBER j = data.getNumber();
        if (j < 1001 && j > -1001 && Value(data.toString()).indexOf(".") == -1) {
          char mode;
          if (j >= 0) {
            mode = 3;
          } else {
            mode = 4;
            j = -j;
          }
          if (j >= 31) {
            mode |= 31 << 3;
            j -= 31;
          } else {
#ifndef USE_DOUBLE
#ifndef USE_BIG_NUMBER
            mode |= j.get_si() << 3;
#else
            mode |= j.toLong() << 3;
#endif
#else
            mode |= (long) j << 3;
#endif
            j = 0;
          }
          res += mode;
          for (; j > 0; j -= 255) {
            if (j >= 255) {
              res += (char) 255;
            } else {
#ifndef USE_DOUBLE
#ifndef USE_BIG_NUMBER
              res += (char) j.get_si();
#else
              res += (char) j.toLong();
#endif
#else
              res += (char) ((long) j);
#endif
            }
          }
          res += (char) 0;
        } else {
          res += (char) 6;
          res += data.toString();
          res += (char) 0;
        }
        break;
      }
      case Types::Text:
        res += (char) 5;
        for (char j : data.toString()) {
          if (j == 0) res += j;
          res += j;
        }
        res += (char) 0;
        break;
    }
  }

  TEXT mkBin(const Value& program) {
    TEXT res;
    bool PUT_DATA = false;
    for (int x = 0; x < program.length(); x++) {
      if (PUT_DATA) {
        PUT_DATA = false;
        appendData(program[x], res);
        continue;
      }
      long l = program[x];
      if (NEEDS_PARAMETER(l)) {
        PUT_DATA = true;
      }
      res += (char) ((long) program[x]);
    }
    return res;
  }
}
#endif
