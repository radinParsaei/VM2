#include <iostream>
#include "VM.h"
#include <fstream>
#include <sstream>
#include "VM_binaries.h"

using namespace std;
using namespace VM_BINARIES;

Value disassemble(int prog, Value val) {
  switch (prog) {
    case OPCODE_PUT:              return TEXT(IS_NUM(val)? "PUT\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "PUT\tBOOL" : (val.getType() == Types::Text? "PUT\tTXT" : "PUT\t")) + val.toString();
    case OPCODE_SETVAR:           return TEXT(IS_NUM(val)? "SETVAR\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "SETVAR\tBOOL" : (val.getType() == Types::Text? "SETVAR\tTXT" : "SETVAR\t")) + val.toString();
    case OPCODE_GETVAR:           return TEXT(IS_NUM(val)? "GETVAR\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "GETVAR\tBOOL" : (val.getType() == Types::Text? "GETVAR\tTXT" : "GETVAR\t")) + val.toString();
    case OPCODE_CREATE_ARR:       return TEXT(IS_NUM(val)? "CREATE_ARR\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "CREATE_ARR\tBOOL" : (val.getType() == Types::Text? "CREATE_ARR\tTXT" : "CREATE_ARR\t")) + val.toString();
    case OPCODE_CREATE_MAP:       return TEXT(IS_NUM(val)? "CREATE_MAP\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "CREATE_MAP\tBOOL" : (val.getType() == Types::Text? "CREATE_MAP\tTXT" : "CREATE_MAP\t")) + val.toString();
    case OPCODE_INCREASE:         return TEXT(IS_NUM(val)? "INCREASE\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INCREASE\tBOOL" : (val.getType() == Types::Text? "INCREASE\tTXT" : "INCREASE\t")) + val.toString();
    case OPCODE_MKFUNC:           return TEXT(IS_NUM(val)? "MKFUNC\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "MKFUNC\tBOOL" : (val.getType() == Types::Text? "MKFUNC\tTXT" : "MKFUNC\t")) + val.toString();
    case OPCODE_CALLFUNC:         return TEXT(IS_NUM(val)? "CALLFUNC\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "CALLFUNC\tBOOL" : (val.getType() == Types::Text? "CALLFUNC\tTXT" : "CALLFUNC\t")) + val.toString();
    case OPCODE_GETPARAM:         return TEXT(IS_NUM(val)? "GETPARAM\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "GETPARAM\tBOOL" : (val.getType() == Types::Text? "GETPARAM\tTXT" : "GETPARAM\t")) + val.toString();
    // case OPCODE_SKIPIF:           return TEXT(IS_NUM(val)? "SKIPIF\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "SKIPIF\tBOOL" : (val.getType() == Types::Text? "SKIPIF\tTXT" : "SKIPIF\t")) + val.toString();
    case OPCODE_SKIPIFN:          return TEXT(IS_NUM(val)? "SKIPIFN\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "SKIPIFN\tBOOL" : (val.getType() == Types::Text? "SKIPIFN\tTXT" : "SKIPIFN\t")) + val.toString();
    case OPCODE_CREATE_CLASS:     return TEXT(IS_NUM(val)? "CREATE_CLASS\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "CREATE_CLASS\tBOOL" : (val.getType() == Types::Text? "CREATE_CLASS\tTXT" : "CREATE_CLASS\t")) + val.toString();
    case OPCODE_CREATE_INSTANCE:  return TEXT(IS_NUM(val)? "CREATE_INSTANCE\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "CREATE_INSTANCE\tBOOL" : (val.getType() == Types::Text? "CREATE_INSTANCE\tTXT" : "CREATE_INSTANCE\t")) + val.toString();
    case OPCODE_CALLFUNCFROMINS:  return TEXT(IS_NUM(val)? "CALLFUNCFROMINS\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "CALLFUNCFROMINS\tBOOL" : (val.getType() == Types::Text? "CALLFUNCFROMINS\tTXT" : "CALLFUNCFROMINS\t")) + val.toString();
    case OPCODE_CALLMETHOD:       return TEXT(IS_NUM(val)? "CALLMETHOD\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "CALLMETHOD\tBOOL" : (val.getType() == Types::Text? "CALLMETHOD\tTXT" : "CALLMETHOD\t")) + val.toString();
    case OPCODE_IS:               return TEXT(IS_NUM(val)? "IS\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "IS\tBOOL" : (val.getType() == Types::Text? "IS\tTXT" : "IS\t")) + val.toString();
    case OPCODE_DECREASE:         return TEXT(IS_NUM(val)? "DECREASE\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "DECREASE\tBOOL" : (val.getType() == Types::Text? "DECREASE\tTXT" : "DECREASE\t")) + val.toString();
    case OPCODE_INPLACE_MUL:      return TEXT(IS_NUM(val)? "INPLACE_MUL\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INPLACE_MUL\tBOOL" : (val.getType() == Types::Text? "INPLACE_MUL\tTXT" : "INPLACE_MUL\t")) + val.toString();
    case OPCODE_INPLACE_DIV:      return TEXT(IS_NUM(val)? "INPLACE_DIV\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INPLACE_DIV\tBOOL" : (val.getType() == Types::Text? "INPLACE_DIV\tTXT" : "INPLACE_DIV\t")) + val.toString();
    case OPCODE_INPLACE_MOD:      return TEXT(IS_NUM(val)? "INPLACE_MOD\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INPLACE_MOD\tBOOL" : (val.getType() == Types::Text? "INPLACE_MOD\tTXT" : "INPLACE_MOD\t")) + val.toString();
    case OPCODE_INPLACE_POW:      return TEXT(IS_NUM(val)? "INPLACE_POW\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INPLACE_POW\tBOOL" : (val.getType() == Types::Text? "INPLACE_POW\tTXT" : "INPLACE_POW\t")) + val.toString();
    case OPCODE_INPLACE_AND:      return TEXT(IS_NUM(val)? "INPLACE_AND\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INPLACE_AND\tBOOL" : (val.getType() == Types::Text? "INPLACE_AND\tTXT" : "INPLACE_AND\t")) + val.toString();
    case OPCODE_INPLACE_OR:       return TEXT(IS_NUM(val)? "INPLACE_OR\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INPLACE_OR\tBOOL" : (val.getType() == Types::Text? "INPLACE_OR\tTXT" : "INPLACE_OR\t")) + val.toString();
    case OPCODE_INPLACE_LSHIFT:   return TEXT(IS_NUM(val)? "INPLACE_LSHIFT\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INPLACE_LSHIFT\tBOOL" : (val.getType() == Types::Text? "INPLACE_LSHIFT\tTXT" : "INPLACE_LSHIFT\t")) + val.toString();
    case OPCODE_INPLACE_RSHIFT:   return TEXT(IS_NUM(val)? "INPLACE_RSHIFT\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INPLACE_RSHIFT\tBOOL" : (val.getType() == Types::Text? "INPLACE_RSHIFT\tTXT" : "INPLACE_RSHIFT\t")) + val.toString();
    case OPCODE_INPLACE_XOR:      return TEXT(IS_NUM(val)? "INPLACE_XOR\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "INPLACE_XOR\tBOOL" : (val.getType() == Types::Text? "INPLACE_XOR\tTXT" : "INPLACE_XOR\t")) + val.toString();
    case OPCODE_DLCALL:           return TEXT(IS_NUM(val)? "DLCALL\tNUM" : (val.getType() == Types::True || val.getType() == Types::False)? "DLCALL\tBOOL" : (val.getType() == Types::Text? "DLCALL\tTXT" : "DLCALL\t")) + val.toString();
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
    case OPCODE_SKIP:             return "SKIP";
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
      cout << disassemble((int) vals[c], (vals.length() < c)? Types::Null:vals[c + 1]).toString() << endl;
      if (NEEDS_PARAMETER((int) vals[c])) c++;
    }
  }
  f.close();
	return 0;
}
