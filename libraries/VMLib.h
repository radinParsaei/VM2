#ifndef LIB_NAME
#error please define LIB_NAME
#endif
#include "VM.h"

#ifdef STATIC_BUILD
#define APPEND2(data1 ,data2) data1 ## _ ## data2
#define APPEND1(data1 ,data2) APPEND2(data1 ,data2)
#define LIB_FUNCTION(FN_NAME) extern "C" Value* APPEND1(APPEND1(bundled_lib, LIB_NAME), FN_NAME) (Value* args, int argc)
#else
#define LIB_FUNCTION(FN_NAME) extern "C" Value* FN_NAME (Value* args, int argc)
#endif