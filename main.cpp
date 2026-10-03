#include "stack_constructor.h"
#include "stdio.h"
#include "string.h"

typedef int (*comporator_t)(const void*, const void*);
void swap(void* a, void* b, size_t el_size, char* buffer);
void buble_sort(void* arr, size_t n, size_t el_size, comporator_t comporator);
int compare_up_s(const void* a, const void* b);


int main() {
	
	StackConstructor a, b, c;
	_init_stack(&c, 1, 4, 2, -3, 3);
	_init_stack(&b);
	_init_stack(&a, 9, 8, 9);
	
	b.merge(&b, &a, &c);
	
	b.pop(&b, 0);
	b.push(&b, 5);
	
	_stack_display(&b);
	
	a.destroy(&a); b.destroy(&b); c.destroy(&c);
	
	
	return 0;
}

