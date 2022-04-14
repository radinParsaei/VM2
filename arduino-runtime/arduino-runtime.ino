#include "VM.h"
#include "VM_binaries.h"

using namespace VM_BINARIES;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  VMStringStream* st = new VMStringStream();
  st->data = (const char[]) { OPCODE_PUT, 5, 'h', 'e', 'l', 'l', 'o', 0, OPCODE_CALLFUNC, 0 };
  st->len = 10;
  st->i = 0;
  VM vm;
  runBinFromStream(*st, vm);
  free(st);
  Serial.println();
  Serial.println();
  // vm.run1(OPCODE_PUT, 0);
  // vm.run1(OPCODE_SETVAR, 0);
  // vm.run1(OPCODE_REC);
  // vm.run1(OPCODE_PUT, "LOOP WORKS!!!\r\n");
  // vm.run1(OPCODE_PRINT);
  // vm.run1(OPCODE_END);
  // vm.run1(OPCODE_REC);
  // vm.run1(OPCODE_PUT, 1);
  // vm.run1(OPCODE_INCREASE, 0);
  // vm.run1(OPCODE_GETVAR, 0);
  // vm.run1(OPCODE_PUT, 10);
  // vm.run1(OPCODE_NEQ);
  // vm.run1(OPCODE_END);
  // vm.run1(OPCODE_WHILE);

  // Serial.println();

  // vm.run1(OPCODE_REC);
  // vm.run1(OPCODE_GETPARAM, 1);
  // vm.run1(OPCODE_PRINT);
  // vm.run1(OPCODE_END);
  // vm.run1(OPCODE_PUT, 1);
  // vm.run1(OPCODE_MKFUNC, "a");
  // vm.run1(OPCODE_PUT, "FUNCTION WORKS!!!\r\n");
  // vm.run1(OPCODE_CALLFUNC, "a");


  // static_call_list.yml:
  // test:
  //   - _: 0
  //   - test: 0

  // lib test: 0
  // test function from lib test: 0
  vm.run1(OPCODE_PUT, 0);
  vm.run1(OPCODE_PUT, 0);
  vm.run1(OPCODE_DLCALL, 0);
  vm.run1(OPCODE_CALLFUNC);

  vm.run1(OPCODE_PUT, 0);
  vm.run1(OPCODE_PUT, 0);
  vm.run1(OPCODE_DLCALL, 0);
  vm.run1(OPCODE_CALLFUNC);

  vm.run1(OPCODE_CALLFUNC, Types::True);
  vm.run1(OPCODE_CALLFUNC);
  }

void loop() {

}