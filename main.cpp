
#include "stack_constructor.h"
#include "stdio.h"

int main() {
	
	StackConstructor a, b, c;
	_init_stack(&a, 1, 4, 2, -3, 3);
	_init_stack(&b, 3, 3, 2, -33, 9);
	_init_stack(&c);
	
	
//	
//	a.push(&a, 1);
//	a.display(&a);
//	a.push(&a, 6);
//	a.push(&a, 6);
//	a.pop(&a, -1);
//	a.push(&a, 7);
//	for (int i = 0; i < 100; i++) {
//		a.push(&a, i);
//		if (i % 2) a.pop(&a, 0);
//		
//		//a.display(&a);	
//	}
	_stack_display(&a);
	
	printf("\n size: %lli \n", a.capacity);
	
	a.extend(&a, 5, 3, -2, 3, 4, 7);
	
	_stack_display(&a);
	
	c.merge(&c, &a, &b);

	
	_stack_display(&c);
	
	a.stack = NULL;
	a.fill(&a, 10, 0);
	_stack_display(&a);
	printf("\n size: %lli \n", a.capacity);
	
	
	return 0;
}
