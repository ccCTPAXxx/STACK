#include "stack_constructor.h"
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>


#define LOGGER 2048
#define LOG_CAPTION "\n-------------------------LOGS-------------------------\n"
#define LOGGER_WRITE(BUFF, reason) fprintf(BUFF, "Reason is: %s\n", reason)

#ifdef DEBUG
#define ON_DEBUG(...) ON_DEBUG(__LINE__, __FILE__, __VA_ARGS__)
#else
#define ON_DEBUG(...)
#endif


typedef long long int lli;

#ifdef KANARY_MODE
const double KANARY = INFINITY;
#endif

static lli closest_2_power(size_t n);
static errno_t xrealloc(StackConstructor* STACK, size_t new_capacity);
static bool is_heap_pointer(StackConstructor* STACK);

/* Thanks to DAN*/
size_t hash_djb2(const StackValue *stack, size_t size) {
	
	#ifdef HASH_MODE
	
	size_t hash = 5381;
	lli c;
	for (size_t i = 0; i < size; i++) {
		c = (size_t) * stack + i;
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
	
	#endif
	
	return 0;
}

static void push_info(const char* info, FILE* file1, FILE* file2, bool mode) {
	if (mode)
		fprintf(file1, "Reason is: ");
	fprintf(file1, info);
	if (mode)
		fprintf(file2, "Reason is: ");
	fprintf(file2, info);
}

#define DUMP(...) dump_stack(__LINE__, __func__, __FILE__, __VA_ARGS__)
static void dump_stack(size_t line, const char* fnc, const char* file, StackConstructor* STACK) {
	assert(fnc != NULL); assert(file != NULL); assert(STACK != NULL);
	
	#ifdef DEBUG_MODE
	FILE* logs = fopen("logs.txt", "a");
	
	time_t now = time(NULL);
	struct tm *timeinfo = localtime(&now);
	
	fprintf(logs, "STACK_DUMP\t[%d:%d:%d]\n called in %s [%s:%lli] {\n",
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, fnc, file, line);
	fprintf(logs, "\t[adress = %p]\n", STACK);
	fprintf(logs, "\t%s -> capacity = %lli\n", STACK -> name, STACK -> capacity);
	fprintf(logs, "\t%s -> size = %lli\n", STACK -> name, STACK -> size);
	
	#ifdef HASH_MODE
	fprintf(logs, "\t%s -> hash = %lli\n", STACK -> name, STACK -> hash);
	#endif
	
	#ifdef KANARY_MODE
	if (is_heap_pointer(STACK))
	fprintf(logs, "\t%s: left_canary -> %lf (etalon = %lf)\n", STACK -> name, ((double*) STACK -> hardware_stack)[0], KANARY);
	else fprintf(logs, "Cannot find canary, stack is broken... ugh...\n");
	#endif
	if (is_heap_pointer(STACK)) {
		fprintf(logs, "\t%s -> stack [adr: %p] {\n", STACK -> name, STACK -> stack);
		for (int i = 0; i < STACK -> size; i++) {
			fprintf(logs, "\t\t[%d] : < %d >\n", i, STACK -> stack[i]);
		}
		fprintf(logs, "\t}\n");
	} else {
		fprintf(logs, "\tstack pointer is NULL\n");
	}
	
	#ifdef KANARY_MODE
	if (is_heap_pointer(STACK))
	fprintf(logs, "\t%s: righ_canary -> %lf (etalon = %lf)\n", STACK -> name, *((double*)((char*)(STACK -> stack) + STACK -> capacity * sizeof(StackValue))), KANARY);
	else fprintf(logs, "Cannot find right canary, stack is broken... ugh...\n");
	#endif
	
	fprintf(logs, "}\n");
	
	fclose(logs);
	#endif
}

static errno_t _check_if_OK_(const char* fname, size_t line, const char* fxname, StackConstructor* STACK, const char* reason) {
	assert(fname != NULL); assert(fxname != NULL); assert(STACK != NULL); assert(reason != NULL);
	
	#ifdef DEBUG_MODE
	DUMP(STACK);
	
	FILE* logs = fopen("logs.txt", "a");
	if (!logs) fprintf(stderr, "AAAAAAAAAAAAAAAA");
	
	push_info(LOG_CAPTION, stdout, logs, 0);
	fprintf(logs, "LOG: im in function %s\t%llu: %s\n", fxname, line, fname);
	fprintf(stdout, "LOG: im in function %s\t%llu: %s\n", fxname, line, fname);
	
	if (STACK -> size > STACK -> capacity) {
		push_info("size > capacity\n", logs, stdout, 0);
		return 1;
	}
	if (STACK -> stack == NULL) {
		push_info("Yo, stack has NULL pointer\n", logs, stdout, 0);
		return 1;
	}
	
	#ifdef KANARY_MODE
	if (STACK -> hardware_stack == NULL) {
		push_info("hardware stack has NULL pointer\n", logs, stdout, 0);
		return 1;
	}
	#endif
	
	if (STACK -> alive != true) {
		push_info("stack is not alive\n", logs, stdout, 0);
		return 1;
	}
	
	#ifdef KANARY_MODE
	if (((double*)(STACK -> hardware_stack))[0] != KANARY || 
		*(double*)((char*)(STACK -> stack) + STACK -> capacity * sizeof(StackValue)) != KANARY) {
		push_info("Kanaries died(((\n", logs, stdout, 0);
		return 1;
	}
	#endif
	
	#ifdef HASH_MODE
	size_t current_hash = hash_djb2(STACK -> stack, STACK -> size);
	if (current_hash != STACK -> hash) {
		push_info("Hash mismatch! Whoose inside???\n", logs, stdout, 0);
		return 1;
	}
	#endif
	
	if (reason[0] != '\0') {
		push_info(reason, logs, stdout, 1);
	}
	#endif
	
	return 0;
}

static errno_t _destroy_(StackConstructor* STACK) {
	assert(STACK != NULL);
	
	char reason[LOGGER] = {};
	errno_t check = _check_if_OK_(__FILE__, __LINE__, __func__, STACK, reason);
	if (check != 0) return check;
	
	assert(STACK != NULL);
	errno = 0;
	
	#ifdef KANARY_MODE
	free(STACK -> hardware_stack);
	STACK -> hardware_stack = NULL;
	#else
	free(STACK -> stack);
	#endif
	
	STACK -> stack = NULL;
	STACK -> size = 0;
	STACK -> capacity = 0;
	STACK -> alive = false;
	
	#ifdef HASH_MODE
	STACK -> hash = 0;
	#endif
	
	if (errno != 0) {
		return errno;
	}
	
	return 0;
}

void _stack_display(StackConstructor* STACK) {
	assert(STACK != NULL);
	
	char reason[LOGGER] = {};
	if (_check_if_OK_(__FILE__, __LINE__, __func__, STACK, reason) != 0) return;
	
	for (lli i = 0; i < STACK -> size; i++) {
		printf("< %d >\t", STACK -> stack[i]);
	}
	putchar('\n');
}

bool _el_in_stack(StackConstructor* STACK, StackValue el) {
	assert(STACK != NULL);
	
	char reason[LOGGER] = {};
	if (_check_if_OK_(__FILE__, __LINE__, __func__, STACK, reason) != 0) return false;
	
	for (lli i = 0; i < STACK -> size; i++) {
		if (STACK -> stack[i] == el) return true;
	}
	return false;
}

bool _stacks_equal(StackConstructor* STACK1, StackConstructor* STACK2) {
	assert(STACK1 != NULL); assert(STACK2 != NULL);
	
	char reason1[LOGGER] = {};
	char reason2[LOGGER] = {};
	if (_check_if_OK_(__FILE__, __LINE__, __func__, STACK1, reason1) != 0) return false;
	if (_check_if_OK_(__FILE__, __LINE__, __func__, STACK2, reason2) != 0) return false;
	
	if (STACK1 -> size != STACK2 -> size) return false;
	
	for (lli i = 0; i < STACK1 -> size; i++) {
		if (STACK1 -> stack[i] != STACK2 -> stack[i]) return false;
	}
	return true;
}

static errno_t _fill_(StackConstructor* STACK, lli leng, StackValue startValue) {
	assert(STACK != NULL);
	
	char reason[LOGGER] = {};
	errno_t check = _check_if_OK_(__FILE__, __LINE__, __func__, STACK, reason);
	if (check != 0) return check;
	
	if (leng > STACK -> capacity) {
		STACK -> capacity = closest_2_power(leng);
		errno_t err = xrealloc(STACK, STACK -> capacity);
		if (err != 0) return err;
	}
	
	for (size_t i = 0; i < (size_t)leng; i++) {
		STACK -> stack[i] = startValue;
	}
	
	STACK -> size = leng;
	
	#ifdef HASH_MODE
	STACK -> hash = hash_djb2(STACK -> stack, STACK -> size);
	#endif
	
	return 0;
}

static errno_t _push_(StackConstructor* STACK, StackValue element) {
	assert(STACK != NULL);
	
	char reason[LOGGER] = {};
	errno_t check = _check_if_OK_(__FILE__, __LINE__, __func__, STACK, reason);
	if (check != 0) return check;
	
	if (STACK -> size + 1 > STACK -> capacity) {
		STACK -> capacity = closest_2_power(STACK -> size + 1);
		errno_t err = xrealloc(STACK, STACK -> capacity);
		if (err != 0) return err;
	}
	STACK -> stack[(STACK -> size)++] = element;
	
	#ifdef HASH_MODE
	STACK -> hash = hash_djb2(STACK -> stack, STACK -> size);
	#endif
	
	return 0;
}

static errno_t _pop_(StackConstructor* STACK, lli index) {
	assert(STACK != NULL);
	
	char reason[LOGGER] = {};
	errno_t check = _check_if_OK_(__FILE__, __LINE__, __func__, STACK, reason);
	if (check != 0) return check;
	
	if (STACK -> size <= 0) return EINVAL;
	
	STACK -> size--;
	
	if (STACK -> size < STACK -> capacity / 4) {
		errno_t err = xrealloc(STACK, STACK -> capacity / 2);
		if (err != 0) return err;
	}
	
	#ifdef HASH_MODE
	STACK -> hash = hash_djb2(STACK -> stack, STACK -> size);
	#endif
	
	return 0;
}

static errno_t _extend_(StackConstructor* STACK, size_t argc, ...) {
	assert(STACK != NULL); assert(argc >= 0);
	
	char reason[LOGGER] = {};
	errno_t check = _check_if_OK_(__FILE__, __LINE__, __func__, STACK, reason);
	if (check != 0) return check;
	
	va_list argv;
	va_start(argv, argc);
	
	if (STACK -> size + (lli)(argc) > STACK -> capacity) {
		STACK -> capacity = closest_2_power(STACK -> size + (lli)(argc));
		errno_t err = xrealloc(STACK, STACK -> capacity);
		if (err != 0) {
			va_end(argv);
			return err;
		}
	}
	
	for (size_t i = 0; i < argc; i++) {
		STACK -> stack[STACK -> size + i] = va_arg(argv, StackValue);
	}
	
	STACK -> size = STACK -> size + (lli)(argc);
	
	#ifdef HASH_MODE
	STACK -> hash = hash_djb2(STACK -> stack, STACK -> size);
	#endif
	
	va_end(argv);
	return 0;
}

static lli closest_2_power(size_t n) {
	for (size_t i = 3; i < SIZE_MAX; i++) {
		if (pow(2, i) > n) {
			return pow(2, i);
		}
	}
	return -1;
}

static errno_t _merge_(StackConstructor* New_Stack, StackConstructor* STACK_1, StackConstructor* STACK_2) {
	assert(New_Stack != NULL); assert(STACK_1 != NULL); assert(STACK_2 != NULL);
	
	char reason1[LOGGER] = {};
	char reason2[LOGGER] = {};
	errno_t check1 = _check_if_OK_(__FILE__, __LINE__, __func__, STACK_1, reason1);
	if (check1 != 0) return check1;
	errno_t check2 = _check_if_OK_(__FILE__, __LINE__, __func__, STACK_2, reason2);
	if (check2 != 0) return check2;
	
	New_Stack -> capacity = closest_2_power(STACK_1 -> size + STACK_2 -> size);
	
	New_Stack -> stack = NULL;
	#ifdef KANARY_MODE
	New_Stack -> hardware_stack = NULL;
	#endif
	
	errno_t err = xrealloc(New_Stack, New_Stack -> capacity);
	if (err != 0) return err;
	
	for (size_t i = 0; i < (size_t)(STACK_1 -> size + STACK_2 -> size); i++) {
		if (i < (size_t)STACK_1 -> size) {
			New_Stack -> stack[i] = STACK_1 -> stack[i];
		} else {
			New_Stack -> stack[i] = STACK_2 -> stack[i - STACK_1 -> size];
		}
	}
	
	New_Stack -> size = STACK_1 -> size + STACK_2 -> size;
	
	#ifdef HASH_MODE
	New_Stack -> hash = hash_djb2(New_Stack -> stack, New_Stack -> size);
	#endif
	
	return 0;
}

static void init_static_vars(StackConstructor* STACK) {
	assert(STACK != NULL);
	
	STACK -> alive = true;
	STACK -> push = _push_;
	STACK -> extend = _extend_;
	STACK -> pop = _pop_;
	STACK -> destroy = _destroy_;
	STACK -> merge = _merge_;
	STACK -> fill = _fill_;
}

errno_t _init_(size_t argc, StackConstructor* STACK, ...) {
	assert(STACK != NULL);
	
	va_list argv;
	va_start(argv, STACK);

	STACK -> capacity = closest_2_power(argc - 1);
	STACK -> size = argc - 1;
	init_static_vars(STACK);
	
	#ifdef KANARY_MODE
	STACK -> hardware_stack = NULL;
	#else
	STACK -> stack = NULL;
	#endif
	
	errno_t err = xrealloc(STACK, STACK -> capacity);
	if (err != 0) {
		va_end(argv);
		return err;
	}
	
	for (size_t i = 0; i < argc - 1; i++) {
		STACK -> stack[i] = va_arg(argv, StackValue);
	}
	
	#ifdef HASH_MODE
	STACK -> hash = hash_djb2(STACK -> stack, STACK -> size);
	#endif
	
	#ifdef DEBUG_MODE
	STACK -> name = "stk";
	STACK -> created_by = "main";
	#endif
	
	va_end(argv);
	return 0;
}

static errno_t xrealloc(StackConstructor* STACK, size_t new_capacity) {
	
	size_t data_size = new_capacity * sizeof(StackValue);
	
	#ifdef KANARY_MODE
	size_t total_size = sizeof(double) + data_size + sizeof(double);
	#else
	size_t total_size = data_size;
	#endif
	
	#ifdef KANARY_MODE
	void* new_hardware_stack = realloc(STACK -> hardware_stack, total_size);
	#else
	StackValue* new_hardware_stack = (StackValue*)realloc(STACK -> stack, total_size);
	#endif
	
	if (!new_hardware_stack) return ENOMEM;
	
	#ifdef KANARY_MODE
	STACK -> hardware_stack = new_hardware_stack;
	((double*)STACK -> hardware_stack)[0] = KANARY;
	STACK -> stack = (StackValue*)((char*)STACK -> hardware_stack + sizeof(double));
	*(double*)((char*)STACK -> stack + data_size) = KANARY;
	#else
	STACK -> stack = new_hardware_stack;
	#endif
	STACK -> capacity = (lli)new_capacity;
	
	return 0;
}

static bool is_heap_pointer(StackConstructor* STACK) {
	#ifdef KANARY_MODE
	void* ptr = STACK -> hardware_stack;
	#else
	void* ptr = STACK -> stack
	#endif
	
	if (ptr == NULL) return false;
	return HeapValidate(GetProcessHeap(), 0, ptr) != 0;
}
