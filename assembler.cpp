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
  } else if (line.startsWith("TXT")) {
    line = line.substring(3);
    line.replace("\\n", "\n");
    line.replace("\\\n", "\\n");
    line.replace("\\t", "\t");
    line.replace("\\\t", "\\t");
    line.replace("\\r", "\r");
    line.replace("\\\r", "\\r");
    line.replace("\\a", "\a");
    line.replace("\\\a", "\\a");
    line.replace("\\b", "\b");
    line.replace("\\\b", "\\b");
    line.replace("\\f", "\f");
    line.replace("\\\f", "\\f");
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
  } else if (line.startsWith("PRINT")) {
    prog.append(OPCODE_PRINT);
  } else if (line.startsWith("IF")) {
    prog.append(OPCODE_IF);
  } else if (line.startsWith("WHILE")) {
    prog.append(OPCODE_WHILE);
  } else if (line.startsWith("REC")) {
    prog.append(OPCODE_REC);
  } else if (line.startsWith("END")) {
    prog.append(OPCODE_END);
  } else if (line.startsWith("NEQ")) {
    prog.append(OPCODE_NEQ);
  }
  return prog;
}

int main(int argc, char const **argv) {
  if(argc < 2){
    cerr << "please enter a file name to assemble\n";
    return 1;
  }
  string filename = "out.bin";
  if(argc >= 3){
    filename = argv[2];
  }
#ifdef USE_GMP_LIB
  mpf_set_default_prec(1024);
#endif
  ofstream ofile;
  ofile.open(filename);
  ifstream fin;
  fin.open(argv[1]);
  Value prog = Types::Array;
  string line;
  while (getline(fin, line)) {
    Value a = assemble(line);
    a.freeMemory = false;
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
