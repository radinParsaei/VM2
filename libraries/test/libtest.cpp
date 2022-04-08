#define LIB_NAME test

#include "VMLib.h"

bool t = false;

LIB_FUNCTION(test) {
    // return 0; // return nothing
    t = !t;
    if (t) {
        return new Value("Hello, World!!!\r\n");
    } else {
        return new Value("Hi, There!\r\n");
    }
}