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
#define OPCODE_DELVAR           10
#define OPCODE_REC              11
#define OPCODE_END              12
#define OPCODE_SKIPIF           13 // used in nested while loops
#define OPCODE_SKIPIFN          14 // skip if top of stack is not true (useful to skip else condition)
#define OPCODE_WHILE            15
#define OPCODE_NEQ              16 // not equal (!=)
#define OPCODE_EQ               17 // equal (==)
#define OPCODE_GT               18 // greater than
#define OPCODE_GE               19 // greater than or equal
#define OPCODE_LT               20 // less than
#define OPCODE_LE               21 // less than or equal
#define OPCODE_LEQ              22 // looks equal
#define OPCODE_AND              23 // AND (&&)
#define OPCODE_OR               24 // OR (||)
#define OPCODE_B_AND            25 // Bitwise AND (&)
#define OPCODE_B_OR             26 // Bitwise OR (|)
#define OPCODE_B_NOT            27 // Bitwise NOT (~)
#define OPCODE_NOT              28 // NOT (!)
#define OPCODE_LSHIFT           29 // left shift (<<)
#define OPCODE_RSHIFT           30 // right shift (>>)
#define OPCODE_XOR              31 // XOR (^)
#define OPCODE_NEGATE           32 // negate (-)
#define OPCODE_POP              33
#define OPCODE_CREATE_ARR       34 // create array
#define OPCODE_GET              35 // get from array (arr[index])
#define OPCODE_SET              36 // set to array (arr[index] = value)
#define OPCODE_CREATE_MAP       37 // create map
#define OPCODE_MKFUNC           38 // create function
#define OPCODE_CALLFUNC         39 // call function
#define OPCODE_GETPARAM         40 // get function parameter
#define OPCODE_CONTINUE         41 // continue loop
#define OPCODE_BREAK            42 // break loop
#define OPCODE_RETURN           43 // break function (return)
#define OPCODE_WHILET           44 // while true
#define OPCODE_SKIP             45 // skip next instructions
#define OPCODE_CREATE_CLASS     46 // create class
#define OPCODE_CREATE_INSTANCE  47 // create instance
#define OPCODE_CALLFUNCFROMINS  48 // call function from instance (call Test#toString())
#define OPCODE_CALLMETHOD       49 // call method from instance (call toString from Test) *finds full function name (something like Test#toString) in runtime
#define OPCODE_THIS             50 // get this/self instance
#define OPCODE_GETPTRTOLASTFUNC 51 // get a pointer to the last defined function
#define OPCODE_IS               52 // check the type of data (is data of type) 0 for Null, 1 for True, 2 for False, 3 for any kind of Boolean, 4 for Number, 5 for Text, 6 for Array, 7 for Map, 8 for any kind of instance and pass the name of class to see if the value is an instance of that class
#define OPCODE_INCREASE         53 // increase variable
#define OPCODE_DECREASE         54 // decrease variable
#define OPCODE_INPLACE_MUL      55 // multiply variable in place
#define OPCODE_INPLACE_DIV      56 // divide variable in place
#define OPCODE_INPLACE_MOD      57 // modulo variable in place
#define OPCODE_INPLACE_POW      58 // power variable in place
#define OPCODE_INPLACE_AND      59 // and variable in place
#define OPCODE_INPLACE_OR       60 // or variable in place
#define OPCODE_INPLACE_LSHIFT   61 // left shift variable in place
#define OPCODE_INPLACE_RSHIFT   62 // left shift variable in place
#define OPCODE_INPLACE_XOR      63 // xor variable in place
#define OPCODE_DLCALL           64 // call a function from a dynamic library


#endif // OPCODES_H