#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stacklib.h"


node* create_node(void *data) // cria a lista encadeada dos elementos da stack
{
    node *new = malloc(sizeof(node));
    new->data = data; 

    return new;
}

stack* create_stack() // cria a stack
{
    stack *new_stk = malloc(sizeof(stack));
    new_stk->top = NULL;
    new_stk->size = 0;

    return new_stk;
}

void push(stack *stk, void *data) // empilha
{
   node *new_top = create_node(data);

   new_top->next = stk->top;
   stk->top = new_top;

   stk->size++;
}

void* pull(stack *stk) // desempilha
{
   if(!stk->size){

    return NULL;

   }

  node *unstacked = stk->top;
  stk->top = stk->top->next;
  unstacked->next = NULL;
  stk->size--;

  void *data = unstacked->data;
  unstacked->data = NULL;
  free(unstacked); // não pode liberar o conteúdo em si

  return data; // contudo retorna

}

void* peek(stack *pilha)
{
    return pilha->top ? pilha->top->data : NULL;
}


void* undo(stack *UNDO, stack *REDO) {

    if(UNDO->size <= 1){

        return NULL;
    }

    void *data = pull(UNDO);

    if (data) {

        push(REDO, data);

        return peek(UNDO); 
    }

    return NULL;
}

void* redo(stack *UNDO, stack *REDO) 
{

    void *data = pull(REDO);

    if (data) {

        push(UNDO, data);
        
        return peek(UNDO);
    }

    return NULL;
}

void free_unitary_stack(stack *stk, void (*free_func)(void *parameter))
{
    node *temp = stk->top;

    while(temp){
        
        node *aux = temp;
        temp = temp->next;

        (*free_func)(aux->data); 
        free(aux);        
    }

    stk->size = 0;
    stk->top = NULL;

}

void free_stacks(stack *UNDO, stack *REDO, void (*free_func)(void *parameter)) // detroi as pilhas
{
    free_unitary_stack(UNDO, free_func);
    free_unitary_stack(REDO, free_func);

    UNDO->top = NULL;
    REDO->top = NULL;

    free(UNDO);
    free(REDO);
}


