#include "VM.h"
#include "VM_binaries.h"

using namespace VM_BINARIES;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  VMStringStream* st = new VMStringStream();
  st->data = (const char[]) { OPCODE_PUT, 5, 'h', 'e', 'l', 'l', 'o', 0, OPCODE_PRINT };
  st->len = 9;
  st->i = 0;
  VM vm;
  runBinFromStream(*st, vm);
  free(st);
  Serial.println();
  // Serial.println();
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

  vm.run1(OPCODE_REC);
  vm.run1(OPCODE_THIS);
  vm.run1(OPCODE_PUT, "a");
  vm.run1(OPCODE_GET);
  vm.run1(OPCODE_END);
  vm.run1(OPCODE_PUT, 0);
  vm.run1(OPCODE_MKFUNC, 0);

  vm.run1(OPCODE_PUT, "#toString");
  vm.run1(OPCODE_GETPTRTOLASTFUNC);

  vm.run1(OPCODE_REC);
  vm.run1(OPCODE_PUT, "Hello!");
  vm.run1(OPCODE_PRINT);
  vm.run1(OPCODE_END);
  vm.run1(OPCODE_PUT, 0);
  vm.run1(OPCODE_MKFUNC, 1);

  vm.run1(OPCODE_PUT, "<init>");
  vm.run1(OPCODE_GETPTRTOLASTFUNC);

  vm.run1(OPCODE_PUT, "a");
  vm.run1(OPCODE_PUT, 10);

  vm.run1(OPCODE_CREATE_MAP, 3);
  vm.run1(OPCODE_CREATE_CLASS, 0);
  vm.run1(OPCODE_CREATE_INSTANCE, 0);
  vm.run1(OPCODE_PRINT);
  Serial.println(".");
}

void loop() {

}