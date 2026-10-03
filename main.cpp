#include "stack_constructor.h"
#include "stdio.h"

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

