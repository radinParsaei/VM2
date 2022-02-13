#include "VM.h"
#include "VM_binaries.h"

using namespace VM_BINARIES;

void setup() {
  Serial.begin(9600);
  VMStringStream* st = new VMStringStream();
  st->data = (const char[]) { OPCODE_PUT, 5, 'h', 'e', 'l', 'l', 'o', 0, OPCODE_PRINT };
  st->len = 9;
  st->i = 0;
  VM vm;
  runBinFromStream(*st, vm);
  free(st);
}

void loop() {

}