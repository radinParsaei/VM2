#include <iostream>
#include "VM.h"
#include <fstream>
#include <sstream>
#include "VM_binaries.h"

using namespace std;
using namespace VM_BINARIES;

Value disassemble(int prog, Value val) {
  switch (prog) {
    case OPCODE_PUT:          return TEXT(IS_NUM(val)? "PUT\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "PUT\tBOOL" : (val.getType() == Types::Text? "PUT\tTXT" : "PUT\t")) + val.toString();
    case OPCODE_SETVAR:       return TEXT(IS_NUM(val)? "SETVAR\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "SETVAR\tBOOL" : (val.getType() == Types::Text? "SETVAR\tTXT" : "SETVAR\t")) + val.toString();
    case OPCODE_GETVAR:       return TEXT(IS_NUM(val)? "GETVAR\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "GETVAR\tBOOL" : (val.getType() == Types::Text? "GETVAR\tTXT" : "GETVAR\t")) + val.toString();
    case OPCODE_CREATE_ARR:   return TEXT(IS_NUM(val)? "CREATE_ARR\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "CREATE_ARR\tBOOL" : (val.getType() == Types::Text? "CREATE_ARR\tTXT" : "CREATE_ARR\t")) + val.toString();
    case OPCODE_CREATE_MAP:   return TEXT(IS_NUM(val)? "CREATE_MAP\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "CREATE_MAP\tBOOL" : (val.getType() == Types::Text? "CREATE_MAP\tTXT" : "CREATE_MAP\t")) + val.toString();
    case OPCODE_INCREASE:     return TEXT(IS_NUM(val)? "INCREASE\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INCREASE\tBOOL" : (val.getType() == Types::Text? "INCREASE\tTXT" : "INCREASE\t")) + val.toString();
    case OPCODE_ADD:          return "ADD";
    case OPCODE_SUB:          return "SUB";
    case OPCODE_MUL:          return "MUL";
    case OPCODE_DIV:          return "DIV";
    case OPCODE_MOD:          return "MOD";
    case OPCODE_POW:          return "POW";
    case OPCODE_REC:          return "REC";
    case OPCODE_END:          return "END";
    case OPCODE_IF:           return "IF";
    case OPCODE_WHILE:        return "WHILE";
    case OPCODE_PRINT:        return "PRINT";
    case OPCODE_GT:           return "GT";
    case OPCODE_GE:           return "GE";
    case OPCODE_LT:           return "LT";
    case OPCODE_LE:           return "LE";
    case OPCODE_POP:          return "POP";
    case OPCODE_EQ:           return "EQ";
    case OPCODE_NEQ:          return "NEQ";
    case OPCODE_LEQ:          return "LEQ";
    case OPCODE_AND:          return "AND";
    case OPCODE_OR:           return "OR";
    case OPCODE_B_OR:         return "BOR";
    case OPCODE_B_AND:        return "BAND";
    case OPCODE_B_NOT:        return "BNOT";
    case OPCODE_NOT:          return "NOT";
    case OPCODE_LSHIFT:       return "LSHIFT";
    case OPCODE_RSHIFT:       return "RSHIFT";
    case OPCODE_XOR:          return "XOR";
    case OPCODE_NEGATE:       return "NEGATE";
    case OPCODE_GET:          return "GET";
    case OPCODE_SET:          return "SET";
    default:                  return "???";
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
