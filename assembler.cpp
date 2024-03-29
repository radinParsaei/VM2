#include <fstream>
#include <iostream>
#include "VM.h"
#include "VM_binaries.h"

using namespace std;
using namespace VM_BINARIES;

void readValue(Value& prog, Value& line) {
  line = line.trimLeft();
  if (line.startsWith("NUM")) {
    line = line.substring(3);
    int i = 0;
    while (!(isdigit(line.charAt(i)) || line.charAt(i) == '.' || line.charAt(i) == '-')) i++;
    line = line.substring(i);
    line = line.trim();
    prog.append(line.toString());
    prog[prog.length() - 1].toNumber();
  } else if (line.startsWith("BNUM")) {
    line = line.substring(4);
    int i = 0;
    while (!(isdigit(line.charAt(i)) || line.charAt(i) == '.' || line.charAt(i) == '-')) i++;
    line = line.substring(i);
    line = line.trim();
    prog.append(NUMBER_FROM_STRING(line.toString().c_str()));
  } else if (line.startsWith("SNUM")) {
    line = line.substring(4);
    int i = 0;
    while (!(isdigit(line.charAt(i)) || line.charAt(i) == '.' || line.charAt(i) == '-')) i++;
    line = line.substring(i);
    line = line.trim();
    prog.append(atof(line.toString().c_str()));
    prog[1].setType(Types::SmallNumber);
  } else if (line.startsWith("TXT")) {
    line = line.substring(3);
    line.replace("\\n", "\n");
    line.replace("\\\n", "\\n");
    line.replace("\\\\n", "\\\n");
    line.replace("\\t", "\t");
    line.replace("\\\t", "\\t");
    line.replace("\\\\t", "\\\t");
    line.replace("\\r", "\r");
    line.replace("\\\r", "\\r");
    line.replace("\\\\r", "\\\r");
    line.replace("\\a", "\a");
    line.replace("\\\a", "\\a");
    line.replace("\\\\a", "\\\a");
    line.replace("\\b", "\b");
    line.replace("\\\b", "\\b");
    line.replace("\\\\b", "\\\b");
    line.replace("\\f", "\f");
    line.replace("\\\f", "\\f");
    line.replace("\\\\f", "\\\f");
    line.replace("\\\'", "\'");
    line.replace("\\\"", "\"");
    line.replace("\\\\", "\\");
    prog.append(line);
  } else if (line.startsWith("BOOL")) {
    line = line.substring(4);
    line = line.trim();
    line.replace("1", "true");
    line.replace("0", "false");
    prog.append(line.toLower() == "true");
  } else {
    prog.append(Types::Null);
  }
}

Value assemble(Value line) {
  Value prog = Types::Array;
  line = line.trimLeft();
  if (line.startsWith("PUT")) {
    prog.append(OPCODE_PUT);
    line = line.substring(3);
    readValue(prog, line);
  } else if (line.startsWith("ADD")) {
    prog.append(OPCODE_ADD);
  } else if (line.startsWith("SUB")) {
    prog.append(OPCODE_SUB);
  } else if (line.startsWith("MUL")) {
    prog.append(OPCODE_MUL);
  } else if (line.startsWith("DIV")) {
    prog.append(OPCODE_DIV);
  } else if (line.startsWith("MOD")) {
    prog.append(OPCODE_MOD);
  } else if (line.startsWith("POW")) {
    prog.append(OPCODE_POW);
  } else if (line.startsWith("SETVAR")) {
    prog.append(OPCODE_SETVAR);
    line = line.substring(6);
    readValue(prog, line);
  } else if (line.startsWith("GETVAR")) {
    prog.append(OPCODE_GETVAR);
    line = line.substring(6);
    readValue(prog, line);
  } else if (line.startsWith("DELVAR")) {
    prog.append(OPCODE_DELVAR);
    line = line.substring(6);
    readValue(prog, line);
  } else if (line.startsWith("#INPUT")) { // true -> input
    prog.append(OPCODE_CALLFUNC);
    prog.append(Types::True);
  } else if (line.startsWith("#FLUSH")) { // -1 -> flush
    prog.append(OPCODE_CALLFUNC);
    prog.append(-1);
  } else if (line.startsWith("#PRINTLN")) { // false -> println
    prog.append(OPCODE_CALLFUNC);
    prog.append(Types::False);
  } else if (line.startsWith("#PRINT")) { // print opcode is removed (instead call Null function (CALLFUNC NULL))
    // prog.append(OPCODE_PRINT);
    prog.append(OPCODE_CALLFUNC);
    prog.append(Types::Null);
  // } else if (line.startsWith("IF")) {
  //   prog.append(OPCODE_IF);
  } else if (line.startsWith("SKIPIFN")) {
    prog.append(OPCODE_SKIPIFN);
    line = line.substring(7);
    readValue(prog, line);
  } else if (line.startsWith("SKIPIF")) {
     prog.append(OPCODE_SKIPIF);
     line = line.substring(6);
     readValue(prog, line);
  } else if (line.startsWith("WHILET")) {
    prog.append(OPCODE_WHILET);
  } else if (line.startsWith("WHILE")) {
    prog.append(OPCODE_WHILE);
  } else if (line.startsWith("REC")) {
    prog.append(OPCODE_REC);
  } else if (line.startsWith("END")) {
    prog.append(OPCODE_END);
  } else if (line.startsWith("EQ")) {
    prog.append(OPCODE_EQ);
  } else if (line.startsWith("NEQ")) {
    prog.append(OPCODE_NEQ);
  } else if (line.startsWith("GT")) {
    prog.append(OPCODE_GT);
  } else if (line.startsWith("GETPARAM")) {
    prog.append(OPCODE_GETPARAM);
    line = line.substring(8);
    readValue(prog, line);
  } else if (line.startsWith("GETPTRTOLASTFUNC")) {
    prog.append(OPCODE_GETPTRTOLASTFUNC);
  } else if (line.startsWith("GET")) {
    prog.append(OPCODE_GET);
  } else if (line.startsWith("GE")) {
    prog.append(OPCODE_GE);
  } else if (line.startsWith("LT")) {
    prog.append(OPCODE_LT);
  } else if (line.startsWith("LE")) {
    prog.append(OPCODE_LE);
  } else if (line.startsWith("POP")) {
    prog.append(OPCODE_POP);
  } else if (line.startsWith("LEQ")) {
    prog.append(OPCODE_LEQ);
  } else if (line.startsWith("AND")) {
    prog.append(OPCODE_AND);
  } else if (line.startsWith("OR")) {
    prog.append(OPCODE_OR);
  } else if (line.startsWith("BOR")) {
    prog.append(OPCODE_B_OR);
  } else if (line.startsWith("BAND")) {
    prog.append(OPCODE_B_AND);
  } else if (line.startsWith("BNOT")) {
    prog.append(OPCODE_B_NOT);
  } else if (line.startsWith("NOT")) {
    prog.append(OPCODE_NOT);
  } else if (line.startsWith("LSHIFT")) {
    prog.append(OPCODE_LSHIFT);
  } else if (line.startsWith("RSHIFT")) {
    prog.append(OPCODE_RSHIFT);
  } else if (line.startsWith("XOR")) {
    prog.append(OPCODE_XOR);
  } else if (line.startsWith("NEGATE")) {
    prog.append(OPCODE_NEGATE);
  } else if (line.startsWith("CREATE_ARR")) {
    prog.append(OPCODE_CREATE_ARR);
    line = line.substring(10);
    readValue(prog, line);
  } else if (line.startsWith("CREATE_MAP")) {
    prog.append(OPCODE_CREATE_MAP);
    line = line.substring(10);
    readValue(prog, line);
  } else if (line.startsWith("SET")) {
    prog.append(OPCODE_SET);
  } else if (line.startsWith("INCREASE")) {
    prog.append(OPCODE_INCREASE);
    line = line.substring(8);
    readValue(prog, line);
  } else if (line.startsWith("MKFUNC")) {
    prog.append(OPCODE_MKFUNC);
    line = line.substring(6);
    readValue(prog, line);
  } else if (line.startsWith("CALLFUNCFROMINS")) {
    prog.append(OPCODE_CALLFUNCFROMINS);
    line = line.substring(16);
    readValue(prog, line);
  } else if (line.startsWith("CALLMETHOD")) {
    prog.append(OPCODE_CALLMETHOD);
    line = line.substring(10);
    readValue(prog, line);
  } else if (line.startsWith("CALLFUNC")) {
    prog.append(OPCODE_CALLFUNC);
    line = line.substring(8);
    readValue(prog, line);
  } else if (line.startsWith("SKIP")) {
    prog.append(OPCODE_SKIP);
    line = line.substring(5);
    readValue(prog, line);
  } else if (line.startsWith("BREAK")) {
    prog.append(OPCODE_BREAK);
  } else if (line.startsWith("CONTINUE")) {
    prog.append(OPCODE_CONTINUE);
  } else if (line.startsWith("RET")) { // RETURN, RET
    prog.append(OPCODE_RETURN);
  } else if (line.startsWith("CREATE_CLASS")) {
    prog.append(OPCODE_CREATE_CLASS);
    line = line.substring(12);
    readValue(prog, line);
  } else if (line.startsWith("CREATE_INSTANCE")) {
    prog.append(OPCODE_CREATE_INSTANCE);
    line = line.substring(15);
    readValue(prog, line);
  } else if (line.startsWith("THIS")) {
    prog.append(OPCODE_THIS);
  } else if (line.startsWith("IS")) {
    prog.append(OPCODE_IS);
    line = line.substring(2);
    readValue(prog, line);
  } else if (line.startsWith("DECREASE")) {
    prog.append(OPCODE_DECREASE);
    line = line.substring(8);
    readValue(prog, line);
  } else if (line.startsWith("INPLACE_MUL")) {
    prog.append(OPCODE_INPLACE_MUL);
    line = line.substring(11);
    readValue(prog, line);
  } else if (line.startsWith("INPLACE_DIV")) {
    prog.append(OPCODE_INPLACE_DIV);
    line = line.substring(11);
    readValue(prog, line);
  } else if (line.startsWith("INPLACE_MOD")) {
    prog.append(OPCODE_INPLACE_MOD);
    line = line.substring(11);
    readValue(prog, line);
  } else if (line.startsWith("INPLACE_POW")) {
    prog.append(OPCODE_INPLACE_POW);
    line = line.substring(11);
    readValue(prog, line);
  } else if (line.startsWith("INPLACE_AND")) {
    prog.append(OPCODE_INPLACE_AND);
    line = line.substring(11);
    readValue(prog, line);
  } else if (line.startsWith("INPLACE_OR")) {
    prog.append(OPCODE_INPLACE_OR);
    line = line.substring(10);
    readValue(prog, line);
  } else if (line.startsWith("INPLACE_LSHIFT")) {
    prog.append(OPCODE_INPLACE_LSHIFT);
    line = line.substring(14);
    readValue(prog, line);
  } else if (line.startsWith("INPLACE_RSHIFT")) {
    prog.append(OPCODE_INPLACE_RSHIFT);
    line = line.substring(14);
    readValue(prog, line);
  } else if (line.startsWith("INPLACE_XOR")) {
    prog.append(OPCODE_INPLACE_XOR);
    line = line.substring(11);
    readValue(prog, line);
  } else if (line.startsWith("DLCALL")) {
    prog.append(OPCODE_DLCALL);
    line = line.substring(6);
    readValue(prog, line);
  }
  return prog;
}

int main(int argc, char const **argv) {
  if (argc < 2) {
    cerr << "please enter a file name to assemble\n";
    return 1;
  }
  string filename = "out.bin";
  if (argc >= 3) {
    filename = argv[2];
  }
#ifndef USE_DOUBLE
#ifndef USE_BIG_NUMBER
  mpf_set_default_prec(1024);
#else
  BigNumber::begin(10);
#endif
#endif
  ofstream ofile;
  ofile.open(filename);
  ifstream fin;
  fin.open(argv[1]);
  Value prog = Types::Array;
  string line;
  while (getline(fin, line)) {
    Value a = assemble(line);
    for (int i = 0; i < a.length(); i++) {
      prog.append(a[i]);
    }
  }
  fin.close();
  cout << "----------SAVING TO " + filename + "----------" << endl;
  ofile << mkBin(prog);
  ofile.close();
  return 0;
}
