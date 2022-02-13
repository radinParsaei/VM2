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

  void runBinFromStream(VMBinStream& in, VM vm) {
    Value program = Types::Array;
    char data = 0;
    bool dataAvailable = in.get(data);
    while (dataAvailable) {
      program.append((int)data);
      if (data == OPCODE_PUT) {
        if (!in.get(data)) break;
        if (data == 0) {
          program.append(Types::Null);
        } else if (data == 1) {
          program.append(Types::True);
        } else if (data == 2) {
          program.append(Types::False);
        } else if ((data & 0b00000111) == 3 || (data & 0b00000111) == 4) { //small numbers
          bool isneg = (data & 0b00000111) - 3;
          NUMBER n = (data & 0b11111000) >> 3;
          while (data != 0) {
            if (!in.get(data)) break;
            n += (unsigned char)data;
          }
          if (isneg) n = -n;
          program.append(n);
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
          program.append(s);
          vm.run(program);
          program.clear();
          continue;
        } else if (data == 6) {
          TEXT s;
          while (data != 0) {
            if (!in.get(data)) break;
            s += data;
          }
          program.append(NUMBER_FROM_STRING(s.c_str()));
        }
      }
      vm.run(program);
      program.clear();
      dataAvailable = (bool)in.get(data);
    }
  }

  Value parseString(VMBinStream& in) {
    Value program = Types::Array;
    char data = 0;
    bool dataAvailable = (bool)(bool)in.get(data);
    while (dataAvailable) {
      program.append((int)data);
      if (data == OPCODE_PUT) {
        if (!in.get(data)) break;
        if (data == 0) {
          program.append(Types::Null);
        } else if (data == 1) {
          program.append(Types::True);
        } else if (data == 2) {
          program.append(Types::False);
        } else if ((data & 0b00000111) == 3 || (data & 0b00000111) == 4) { //small numbers
          bool isneg = (data & 0b00000111) - 3;
          NUMBER n = (data & 0b11111000) >> 3;
          while (data != 0) {
            if (!in.get(data)) break;
            n += (unsigned char)data;
          }
          if (isneg) n = -n;
          program.append(n);
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
          program.append(s);
          continue;
        } else if (data == 6) {
          TEXT s;
          while (data != 0) {
            if (!in.get(data)) break;
            s += data;
          }
          program.append(NUMBER_FROM_STRING(s.c_str()));
        }
      }
      dataAvailable = (bool)in.get(data);
    }
    return program;
  }

  TEXT mkBin(Value program) {
    TEXT res;
    bool PUT_DATA = false;
    for (int x = 0; x < program.length(); x++) {
      if (PUT_DATA) {
        PUT_DATA = false;
        switch (program[x].getType()) {
          case Types::Null:
            res += (char)0;
            continue;
          case Types::True:
            res += (char)1;
            continue;
          case Types::False:
            res += (char)2;
            continue;
          case Types::Number: {
            NUMBER j = program[x].getNumber();
            if (j < 765 /* 3 * 255 (char size) */ && Value(program[x].toString()).indexOf(".") == -1) {
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
                  res += (char)255;
                } else {
#ifndef USE_DOUBLE
#ifndef USE_BIG_NUMBER
                  res += (char)j.get_si();
#else
                  res += (char)j.toLong();
#endif
#else
                  res += (char) ((long) j);
#endif
                }
              }
              res += (char)0;
            } else {
              res += (char)6;
              res += program[x].toString();
              res += (char)0;
            }
            continue;
          }
          case Types::Text:
            res += (char)5;
            for (char j : program[x].toString()) {
              if (j == 0) res += j;
              res += j;
            }
            res += (char)0;
            continue;
        }
      }
      if ((long) program[x] == OPCODE_PUT) {
        PUT_DATA = true;
      }
      res += (char) ((long) program[x]);
    }
    return res;
  }
}
#endif
