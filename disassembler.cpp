#include <iostream>
#include "VM.h"
#include <fstream>
#include <sstream>
#include "VM_binaries.h"

using namespace std;
using namespace VM_BINARIES;

Value disassemble(int prog, Value val) {
  switch (prog) {
    case OPCODE_PUT:     return TEXT(val.getType() == Types::Number? "PUT\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "PUT\tBOOL" : (val.getType() == Types::Text? "PUT\tTXT" : "PUT\t")) + val.toString();
    case OPCODE_ADD:     return "ADD";
    case OPCODE_SUB:     return "SUB";
    case OPCODE_MUL:     return "MUL";
    case OPCODE_DIV:     return "DIV";
    // case OPCODE_MOD:     return "MOD";
    case OPCODE_PRINT:   return "PRINT";
    default:             return "???";
    return 0;
  }
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    cerr << "please enter a binary file name\n";
    return 1;
  }
  ifstream f(argv[1], ios::binary);
  if(!f){
    cerr << "can't open file" << '\n';
    return 1;
  }
#ifdef USE_GMP_LIB
  mpf_set_default_prec(1024);
#endif
  VMStringStream* st = new VMStringStream();
  string s = string((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
  st->data = s.c_str();
  st->len = s.length();
  Value vals = parseString(*st);
  for(int c = 0; c < vals.length(); c++) {
    if (vals[c].getType() == Types::Number) {
      cout << disassemble((int) vals[c], (vals.length() - 1 == c)? Types::Null:vals[c + 1]).toString() << endl;
      if (vals[c].getNumber() == OPCODE_PUT) c++;
    }
  }
  f.close();
	return 0;
}
