#ifndef OPCODES_H
#define OPCODES_H

#define OPCODE_PRINT            100
#define OPCODE_PUT              1
#define OPCODE_ADD              2
#define OPCODE_SUB              3
#define OPCODE_MUL              4
#define OPCODE_DIV              5
#define OPCODE_MOD              6
#define OPCODE_POW              7
#define OPCODE_SETVAR           8
#define OPCODE_GETVAR           9
#define OPCODE_REC              10
#define OPCODE_END              11
#define OPCODE_IF               12
#define OPCODE_WHILE            13
#define OPCODE_NEQ              14 // not equal (!=)
#define OPCODE_EQ               15 // equal (==)
#define OPCODE_GT               16 // greater than
#define OPCODE_GE               17 // greater than or equal
#define OPCODE_LT               18 // less than
#define OPCODE_LE               19 // less than or equal
#define OPCODE_LEQ              21 // looks equal
#define OPCODE_AND              22 // AND (&&)
#define OPCODE_OR               23 // OR (||)
#define OPCODE_B_AND            24 // Bitwise AND (&)
#define OPCODE_B_OR             25 // Bitwise OR (|)
#define OPCODE_B_NOT            26 // Bitwise NOT (~)
#define OPCODE_NOT              27 // NOT (!)
#define OPCODE_LSHIFT           28 // left shift (<<)
#define OPCODE_RSHIFT           29 // right shift (>>)
#define OPCODE_XOR              30 // XOR (^)
#define OPCODE_NEGATE           31 // negate (-)
#define OPCODE_POP              32
#define OPCODE_CREATE_ARR       33 // create array
#define OPCODE_GET              34 // get from array (arr[index])
#define OPCODE_SET              35 // set to array (arr[index] = value)
#define OPCODE_CREATE_MAP       36 // create map
#define OPCODE_INCREASE         37 // increase variable
#define OPCODE_MKFUNC           38 // create function
#define OPCODE_CALLFUNC         39 // call function
#define OPCODE_GETPARAM         40 // get function parameter
#define OPCODE_CONTINUE         41 // continue loop
#define OPCODE_BREAK            42 // break loop
#define OPCODE_RETURN           43 // break function (return)

#endif // OPCODES_H