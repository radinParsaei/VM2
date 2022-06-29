#ifndef OPCODES_H
#define OPCODES_H

// #define OPCODE_PRINT            100
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
// #define OPCODE_IF            12
#define OPCODE_SKIPIF           12 // used in nested while loops
#define OPCODE_SKIPIFN          13 // skip if top of stack is not true (useful to skip else condition)
#define OPCODE_WHILE            14
#define OPCODE_NEQ              15 // not equal (!=)
#define OPCODE_EQ               16 // equal (==)
#define OPCODE_GT               17 // greater than
#define OPCODE_GE               18 // greater than or equal
#define OPCODE_LT               19 // less than
#define OPCODE_LE               20 // less than or equal
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
#define OPCODE_MKFUNC           37 // create function
#define OPCODE_CALLFUNC         38 // call function
#define OPCODE_GETPARAM         39 // get function parameter
#define OPCODE_CONTINUE         40 // continue loop
#define OPCODE_BREAK            41 // break loop
#define OPCODE_RETURN           42 // break function (return)
#define OPCODE_WHILET           43 // while true
#define OPCODE_SKIP             44 // skip next instructions
#define OPCODE_CREATE_CLASS     45 // create class
#define OPCODE_CREATE_INSTANCE  46 // create instance
#define OPCODE_CALLFUNCFROMINS  47 // call function from instance (call Test#toString())
#define OPCODE_CALLMETHOD       48 // call method from instance (call toString from Test) *finds full function name (something like Test#toString) in runtime
#define OPCODE_THIS             49 // get this/self instance
#define OPCODE_GETPTRTOLASTFUNC 50 // get a pointer to the last defined function
#define OPCODE_IS               51 // check the type of data (is data of type) 0 for Null, 1 for True, 2 for False, 3 for any kind of Boolean, 4 for Number, 5 for Text, 6 for Array, 7 for Map, 8 for any kind of instance and pass the name of class to see if the value is an instance of that class
#define OPCODE_INCREASE         52 // increase variable
#define OPCODE_DECREASE         53 // decrease variable
#define OPCODE_INPLACE_MUL      54 // multiply variable in place
#define OPCODE_INPLACE_DIV      55 // divide variable in place
#define OPCODE_INPLACE_MOD      56 // modulo variable in place
#define OPCODE_INPLACE_POW      57 // power variable in place
#define OPCODE_INPLACE_AND      58 // and variable in place
#define OPCODE_INPLACE_OR       59 // or variable in place
#define OPCODE_INPLACE_LSHIFT   60 // left shift variable in place
#define OPCODE_INPLACE_RSHIFT   61 // left shift variable in place
#define OPCODE_INPLACE_XOR      62 // xor variable in place
#define OPCODE_DLCALL           63 // call a function from a dynamic library


#endif // OPCODES_H