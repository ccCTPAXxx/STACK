#ifndef STACK_CONSTRUCTOR_H
#define STACK_CONSTRUCTOR_H

//#define HASH_MODE // -D
//#define DEBUG_MODE
//#define KANARY_MODE


#include "stack_constructor.h"
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>

typedef int StackValue;
typedef long long int lli;

enum ERRORS {
	OK,
	SIZE_TOO_BIG,
	SIZE_LESS_0,
	CAPASITY_LESS_0,
	STACK_NULL,
	HARDWARE_NULL,
	STACK_DEAD,
	KANARI_LEFT_DIED,
	KANARI_RIGHT_DIED,
	HASH_ERROR
};

struct StackConstructor {
	errno_t (*init) (size_t argc, StackConstructor* STACK, ...);
	errno_t (*push) (StackConstructor* STACK, StackValue element);
	errno_t (*extend) (StackConstructor* STACK, size_t argc, ...);
	errno_t (*pop) (StackConstructor* STACK, lli index);
	errno_t (*destroy) (StackConstructor* STACK);
	errno_t (*merge) (StackConstructor* STACK, StackConstructor* STACK_1, StackConstructor* STACK_2);
	errno_t (*fill) (StackConstructor* STACK, lli leng, StackValue startValue);
	lli size;
	lli capacity;
	bool alive = false;
	StackValue* stack;
	
	#ifdef HASH_MODE
	size_t hash;
	#endif
	
	#ifdef KANARY_MODE 
	void* hardware_stack; 
	#endif
	
	#ifdef DEBUG_MODE
	const char* name;
	const char* created_by;
	#endif
};

errno_t _stack_display (StackConstructor* Stack);
bool _el_in_stack(StackConstructor* Stack, StackValue el);
bool _stacks_equal(StackConstructor* STACK1, StackConstructor* STACK2);
bool _is_stack_empty(StackConstructor* STACK);

errno_t _init_(size_t argc, StackConstructor* STACK, ...);

#define PP_ARG_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N
#define PP_NARG(...) PP_ARG_N(__VA_ARGS__, 10,9,8,7,6,5,4,3,2,1,0)
#define _init_stack(...) _init_(PP_NARG(__VA_ARGS__), __VA_ARGS__)

#endif
