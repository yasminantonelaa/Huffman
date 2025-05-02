#ifndef STACKLIB_H
#define STACKLIB_H

typedef struct node {
    void *data;
    struct node *next;
} node;

typedef struct {
    int size;
    node *top;
} stack;


stack* create_stack();
void push(stack *stk, void *data);
void* pull(stack *stk);
void* undo(stack *UNDO, stack *REDO);
void* redo(stack *UNDO, stack *REDO);
void free_unitary_stack(stack *stk, void (*free_func)(void *parameter));
void free_stacks(stack *UNDO, stack *REDO, void (*free_func)(void *parameter));

#endif