#include <iostream>
#include "VM.h"
#include <fstream>
#include <sstream>
#include "VM_binaries.h"

using namespace std;
using namespace VM_BINARIES;

inline void addValue(TEXT& t, const Value& val) {
  if (val.getType() == Types::Number) {
        t += "NUM";
      } else if (val.getType() == Types::SmallNumber) {
        t += "SNUM";
      } else if (val.getType() == Types::BigNumber) {
        t += "BNUM";
      } else if (val.getType() == Types::Text) {
        t += "TXT";
      } else if (val.getType() == Types::True || val.getType() == Types::False) {
        t += "BOOL";
      }
      t += val.toString();
}

Value disassemble(int prog, Value val) {
  switch (prog) {
    case OPCODE_PUT:              { TEXT t = "PUT\t"; addValue(t, val); return t; }
    case OPCODE_SETVAR:           { TEXT t = "SETVAR\t"; addValue(t, val); return t; }
    case OPCODE_GETVAR:           { TEXT t = "GETVAR\t"; addValue(t, val); return t; }
    case OPCODE_CREATE_ARR:       { TEXT t = "CREATE_ARR\t"; addValue(t, val); return t; }
    case OPCODE_CREATE_MAP:       { TEXT t = "CREATE_MAP\t"; addValue(t, val); return t; }
    case OPCODE_INCREASE:         { TEXT t = "INCREASE\t"; addValue(t, val); return t; }
    case OPCODE_MKFUNC:           { TEXT t = "MKFUNC\t"; addValue(t, val); return t; }
    case OPCODE_CALLFUNC:         { TEXT t = "CALLFUNC\t"; addValue(t, val); return t; }
    case OPCODE_GETPARAM:         { TEXT t = "GETPARAM\t"; addValue(t, val); return t; }
    case OPCODE_SKIPIFN:          { TEXT t = "SKIPIFN\t"; addValue(t, val); return t; }
    case OPCODE_SKIP:             { TEXT t = "SKIP\t"; addValue(t, val); return t; }
    case OPCODE_CREATE_CLASS:     { TEXT t = "CREATE_CLASS\t"; addValue(t, val); return t; }
    case OPCODE_CREATE_INSTANCE:  { TEXT t = "CREATE_INSTANCE\t"; addValue(t, val); return t; }
    case OPCODE_CALLFUNCFROMINS:  { TEXT t = "CALLFUNCFROMINS\t"; addValue(t, val); return t; }
    case OPCODE_CALLMETHOD:       { TEXT t = "CALLMETHOD\t"; addValue(t, val); return t; }
    case OPCODE_IS:               { TEXT t = "IS\t"; addValue(t, val); return t; }
    case OPCODE_DECREASE:         { TEXT t = "DECREASE\t"; addValue(t, val); return t; }
    case OPCODE_INPLACE_MUL:      { TEXT t = "INPLACE_MUL\t"; addValue(t, val); return t; }
    case OPCODE_INPLACE_DIV:      { TEXT t = "INPLACE_DIV\t"; addValue(t, val); return t; }
    case OPCODE_INPLACE_MOD:      { TEXT t = "INPLACE_MOD\t"; addValue(t, val); return t; }
    case OPCODE_INPLACE_POW:      { TEXT t = "INPLACE_POW\t"; addValue(t, val); return t; }
    case OPCODE_INPLACE_AND:      { TEXT t = "INPLACE_AND\t"; addValue(t, val); return t; }
    case OPCODE_INPLACE_OR:       { TEXT t = "INPLACE_OR\t"; addValue(t, val); return t; }
    case OPCODE_INPLACE_LSHIFT:   { TEXT t = "INPLACE_LSHIFT\t"; addValue(t, val); return t; }
    case OPCODE_INPLACE_RSHIFT:   { TEXT t = "INPLACE_RSHIFT\t"; addValue(t, val); return t; }
    case OPCODE_INPLACE_XOR:      { TEXT t = "INPLACE_XOR\t"; addValue(t, val); return t; }
    case OPCODE_DLCALL:           { TEXT t = "DLCALL\t"; addValue(t, val); return t; }
    case OPCODE_ADD:              return "ADD";
    case OPCODE_SUB:              return "SUB";
    case OPCODE_MUL:              return "MUL";
    case OPCODE_DIV:              return "DIV";
    case OPCODE_MOD:              return "MOD";
    case OPCODE_POW:              return "POW";
    case OPCODE_REC:              return "REC";
    case OPCODE_END:              return "END";
    // case OPCODE_IF:               return "IF";
    case OPCODE_WHILE:            return "WHILE";
    // case OPCODE_PRINT:            return "PRINT";
    case OPCODE_GT:               return "GT";
    case OPCODE_GE:               return "GE";
    case OPCODE_LT:               return "LT";
    case OPCODE_LE:               return "LE";
    case OPCODE_POP:              return "POP";
    case OPCODE_EQ:               return "EQ";
    case OPCODE_NEQ:              return "NEQ";
    case OPCODE_LEQ:              return "LEQ";
    case OPCODE_AND:              return "AND";
    case OPCODE_OR:               return "OR";
    case OPCODE_B_OR:             return "BOR";
    case OPCODE_B_AND:            return "BAND";
    case OPCODE_B_NOT:            return "BNOT";
    case OPCODE_NOT:              return "NOT";
    case OPCODE_LSHIFT:           return "LSHIFT";
    case OPCODE_RSHIFT:           return "RSHIFT";
    case OPCODE_XOR:              return "XOR";
    case OPCODE_NEGATE:           return "NEGATE";
    case OPCODE_GET:              return "GET";
    case OPCODE_SET:              return "SET";
    case OPCODE_BREAK:            return "BREAK";
    case OPCODE_CONTINUE:         return "CONTINUE";
    case OPCODE_RETURN:           return "RETURN";
    case OPCODE_WHILET:           return "WHILET";
    case OPCODE_THIS:             return "THIS";
    case OPCODE_GETPTRTOLASTFUNC: return "GETPTRTOLASTFUNC";
    default:                      return "???";
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
#ifndef USE_DOUBLE
#ifndef USE_BIG_NUMBER
  mpf_set_default_prec(1024);
#else
  BigNumber::begin(10);
#endif
#endif
  VMStringStream* st = new VMStringStream();
  string s = string((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
  st->data = s.c_str();
  st->len = s.length();
  Value vals = parseString(*st);
  for(int c = 0; c < vals.length(); c++) {
    if (vals[c].getType() == Types::Number) {
      cout << disassemble((int) vals[c], (vals.length() < c)? Types::Null:vals[c + 1]).toString() << endl;
      if (NEEDS_PARAMETER((int) vals[c])) c++;
    }
  }
  f.close();
	return 0;
}
