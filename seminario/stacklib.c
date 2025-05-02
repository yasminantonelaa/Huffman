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
/*
Explicação do UNDO: Incialmente é alocado uma cópia da imagem no undo, o deep copy, essa pocição da pilha não pode ser alterada
Após isso toda modificação é empilhada na pilha do UNDO, quando é chado o comando undo essa infomação recente que acobou de ser
empilhada no undo vai para a pilha do redo e proximo topo (informação anterior) é apenas "exibido".
Lembrando que para dar undo deve ter (obviamente) no minímo dois arquivos na pilha, onde um sérá empilhado em  REDO e o outo exibido

*/

void* undo(stack *UNDO, stack *REDO) {

    if(UNDO->size <= 1){

        return NULL;
    }

    void *data = pull(UNDO);

    if (data) {

        push(REDO, data);

        return UNDO->top ? UNDO->top->data : NULL; // so mais uma etapa de seguraça para evitar acesso inválido 
    }

    return NULL;
}
/*Expilcação do REDO: Ao chamar REDO o contéudo do topo da pilha (se houver) é desempilhado e empilhado em UNDO e em seguida
o topo da pilha de UNDO (conteúdo recém desempilhado de REDO) é exibido */

void* redo(stack *UNDO, stack *REDO) {

    void *data = pull(REDO);

    if (data) {

        push(UNDO, data);
        
        return UNDO->top ? UNDO->top->data : NULL; // sepre terá um undo top mas por medida de seguraça tem esse guard
    }

    return NULL;
}

/*Nota: Caso não esteja familiridos com essa prática: é passado um ponteiro para função pois essa lib não tem condições de 
desalocar a memoria do contúdo data (pois é um ponteiro para void) então ela "terceiriza" uma função de desalocar genérica 
do código principal*/

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


