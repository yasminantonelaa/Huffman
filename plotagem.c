#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct node{
    int value;
    struct node *next;
}node;

typedef struct{

    node *head;

}queue;

// lista encadeada

typedef struct{
    int *data;
    int total_size, current_size;
}heap;

// fuções de fila de prioridade com lista 
void enqueue_list(queue *fila, int value)
{
    node *new_node = (node*) malloc(sizeof(node));
    new_node->value = value;

    if (!fila->head || (value < fila->head->value)) {

        new_node->next = fila->head;
        fila->head = new_node;

    } else {

        node *current = fila->head;

        while ((current->next != NULL) &&
              (current->next->value < value)) {
            
            current = current->next;

        }

        new_node->next = current->next;
        current->next = new_node;
    }
}

void enqueue_list_cmp(queue *fila, int value, int *comp)
{
    node *new_node = (node*) malloc(sizeof(node));
    new_node->value = value;

    (*comp)++; // comparações

    if (!fila->head || (value < fila->head->value)) {

        new_node->next = fila->head;
        fila->head = new_node;

    } else {

        node *current = fila->head;

        while ((current->next != NULL) &&
              (current->next->value < value)) {
            
                (*comp)++; // comparações

            current = current->next;

        }

        new_node->next = current->next;
        current->next = new_node;
    }
}

void desaloc_queue(queue *fila)
{
    node *temp = fila->head;

    while(temp){

        node *aux = temp;
        temp = temp->next;
        free(aux);
    }

    fila->head = NULL;
}

queue* destroy_L_queue(queue *fila){

    free(fila);
    return NULL;
}

queue* create_L_queue()
{
    queue *new = malloc(sizeof(queue));
    new->head = NULL;
    return new;
}

// funções de fila de prioridade com heap

heap* alloc_heap(int size) 
{
    heap *new = malloc(sizeof(heap)); 
    new->data = malloc((size + 1) * sizeof(int)); 
    new->total_size = size + 1; 
    new->current_size = 0; 

    return new;
}

void swap(int *a, int *b)
{
    int z; 

    z = *a; 
    *a = *b; 
    *b = z; 
}

int get_parent_index(heap *hp, int i) 
{
    return i >> 1;
}

void enque_heap(heap *hp, int item) 
{
    if (hp->current_size >= hp->total_size)
    {
        printf("Heap Overflow");
        return;
    }

    hp->data[++hp->current_size] = item; 

    int key_index = hp->current_size; 
    int parent_index = get_parent_index(hp, hp->current_size); 

    while (parent_index > 0 &&
        hp->data[key_index] >  hp->data[parent_index]) {

        swap(&hp->data[key_index], &hp->data[parent_index]); 

        key_index = parent_index;

        parent_index = get_parent_index(hp, key_index); 
    }
}

void enque_heap_cpm(heap *hp, int item, int *comp) 
{
    if (hp->current_size >= hp->total_size)
    {
        printf("Heap Overflow");
        return;
    }

    hp->data[++hp->current_size] = item; 

    int key_index = hp->current_size; 
    int parent_index = get_parent_index(hp, hp->current_size); 

    (*comp)++; // comparações !!
  
    while (parent_index > 0 &&
        hp->data[key_index] >  hp->data[parent_index]) {

            (*comp)++; // comparações !!!
        
        swap(&hp->data[key_index], &hp->data[parent_index]); 

        key_index = parent_index;

        parent_index = get_parent_index(hp, key_index); 
    }


}

heap* destroy_heap(heap *hp)
{
    free(hp->data);
    free(hp);
    return NULL;
}

/// fim das funções de heap e lista incio das funções de comparações

int main()
{
    FILE *CSV = fopen("coparacoes.csv", "w");

    if(!CSV){
        printf("ERRO AO CRIAR ARQUIVO\n");
        exit(1);
    }

    fprintf(CSV, "Numero,FilaSemHeap,FilaComHeap\n");

    int max_n = (1 << 16) + 1; // o tamnho maximo sera 
    heap *fila_heap = alloc_heap(max_n);
    queue *fila_lista = create_L_queue();

    
    int n_elements = 1;

    for (int P = 0; P < 16; P++) {
        int alvo = (1 << P) + 1;
      

        int comp_lista = 0, comp_heap = 0;

        while (n_elements < alvo) {
            enque_heap(fila_heap, n_elements);
            enqueue_list(fila_lista, n_elements);
            n_elements++;
         }

        enque_heap_cpm(fila_heap, n_elements, &comp_heap);
        enqueue_list_cmp(fila_lista, n_elements, &comp_lista);

        n_elements++; 

        fprintf(CSV, "%d,%d,%d\n",alvo, comp_lista, comp_heap);
    }

   
    fila_heap = destroy_heap(fila_heap);
    desaloc_queue(fila_lista);
    destroy_L_queue(fila_lista);

    fclose(CSV);

    return 0;
}