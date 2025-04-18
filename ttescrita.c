#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define tam 256

typedef unsigned char u_char;
// *void?????? in all data structs
typedef struct no_htr{

    u_char byte; 
    int freq; 
    double freq_prcnt; //frequencia porcentagem
    struct no_htr *left;
    struct no_htr *right;

}no_htr;// no_htr = no huffman tree

typedef struct{

    no_htr *arr_bytes[tam]; 
    bool stts[tam]; // true se exite e false se não existe;

}freq_arr;

typedef struct{

    int size, max_heap_size;
    no_htr **data; 

}heap;

heap* assembly_heap(int size) // aloca a heap
{
    heap *new = malloc(sizeof(heap));
    new->data = malloc((size + 1) * sizeof(no_htr *)); // lembrando que a heap começa em 1,, deve ser desalocado
    new->max_heap_size = size + 1;
    new->size = 0;

    return new;
}

no_htr* assembly_no_htr(u_char byte)
{
    no_htr *new = malloc(sizeof(no_htr));

    new->byte = byte;
    new->freq = 0;
    new->left = NULL;
    new->right = NULL;

    return new;
}

freq_arr* assembly_freq_arr()
{
    freq_arr *new = malloc(sizeof(freq_arr));
    
    for(int i = 0; i < tam; i++){

        new->arr_bytes[i] = NULL;
        new->stts[i] = false;
    }

    return new;
}

void swap(no_htr **a, no_htr **b){

    no_htr *z;

    z = *a;
    *a = *b;
    *b = z;
}

int get_parent_index(heap *hp, int i)
{
    return i >> 1; // i/2
}

int get_left_index(heap *hp, int i)
{
    return i << 1; // i * 2
}

int get_right_index(heap *hp, int i){

    return (i << 1) + 1; // i * 2 + 1
}

void max_heapfy(heap *hp, int i)
{
    int largest;
    int left_index = get_left_index(hp,i);
    int right_index = get_right_index(hp, i);

    if(left_index <= hp->size &&
       hp->data[left_index]->freq > hp->data[i]->freq){
        largest = left_index;

    } else{

        largest = i;
    }

    if(right_index <= hp->size &&
       hp->data[right_index]->freq > hp->data[largest]->freq){ 

        largest = right_index;

    }

    if(hp->data[i]->freq != hp->data[largest]->freq){

        swap(&hp->data[i], &hp->data[largest]);
        max_heapfy(hp,largest);
    }

}

void enqueue(heap *hp, no_htr *item)
{
    if(hp->size >= hp->max_heap_size)
    {
        printf("Heap Overflow");
        return;
    }

    hp->data[++hp->size] = item;

    int key_index = hp->size;
    int parent_index = get_parent_index(hp, hp->size);

    while(parent_index > 0 &&
          hp->data[key_index]->freq > hp->data[parent_index]->freq){ 

        swap(&hp->data[key_index], &hp->data[parent_index]); 

        key_index = parent_index;

        parent_index = get_parent_index(hp, key_index);
    }

}

void heap_sort(heap *hp){

    int original_size = hp->size;

    for(int i = hp->size; i >= 2; i--){

        swap(&hp->data[1], &hp->data[i]); 

        hp->size--;

        max_heapfy(hp, 1);
    }

    hp->size = original_size;
}

void ler(char arq[]){

    u_char c; // usar unsigned char para ler pq o char simples terar um bit ou byte, sla, para o final

    FILE *file = fopen(arq, "rb");
     
    if(!file){

        printf("ERRO LEITURA");

    }

    int qtd_bytes = 0; // verifica o tamnho do aquivo em bytes
    int arr_size = 0; // verifica qual o tamnho do array de bytes que será mandado para heap em caso de arquivos pequenos

    freq_arr *freq = assembly_freq_arr();

    while(fread(&c, sizeof(u_char), 1, file) == 1){

       if (!freq->stts[(int)c]){

            freq->arr_bytes[(int)c] = assembly_no_htr(c);
            freq->stts[(int)c] = true;
            arr_size++;
       }

       freq->arr_bytes[(int)c]->freq++;
       qtd_bytes++;

    }

    fclose(file); // por algum motivo a equipe do leandro alocava tudo na heap, so sei que em algum momento o arquivo terá que ser relido
    // pois alocar tudo na heap deveria ser crime para dizer o mínimo

    heap *hp = assembly_heap(arr_size);

    for(int i = 0; i < tam; i++){
        
        if(freq->stts[i]){

            freq->arr_bytes[i]->freq_prcnt = ((double)freq->arr_bytes[i]->freq / qtd_bytes) * 100;
            enqueue(hp,freq->arr_bytes[i]);
            freq->arr_bytes[i] = NULL; // nesso ponto os nós das arvores alocados durante a leitura são liberados
        }

    }

    heap_sort(hp);

    // a esse ponto temos a fila da frequencia de bytes ordenada do ruffman;

    free(freq); // desaloca o array incial usado para a leitura

    int soma_freq = 0; // valor de verificação se todos os bits foram lidos
    double soma_percent = 0; // valor para verificar se as porcentagen atribuidas batem

    
    for(int j = 1; j < arr_size + 1; j++){ // lebrando que os indices da heap começam em 1 e terminam no tamanho

        printf("BYTE: %d, Freq %d, percent %lf\n", (int)hp->data[j]->byte, hp->data[j]->freq, hp->data[j]->freq_prcnt);
        soma_freq += hp->data[j]->freq;
        soma_percent += hp->data[j]->freq_prcnt;
        free(hp->data[j]);
    }

    free(hp->data);
    free(hp);

    printf("\n");

    printf("Soma dos Bytes da frequncia %d bytes\n", soma_freq);
    printf("Soma da porcentagens da frequencia %lf\n", soma_percent);
    printf("Tamanho do arquivo lido %d bytes\n", qtd_bytes);
    //fflush(stdout); em algum momento eu estva sofrendo com bug da saida não ser impressa,
    // caso ocorra colocar isso ou \n no final para limpar o buffer e geralmente se rodar denovo dar certo
    
}

int main()
{
    char arquivo[100]; // arquivo de teste

    printf("Digite o nome do arquivo: \n");

    scanf("%99[^\n]", arquivo); // ler 99 carecteres até que seja digitado um \n, não sei se o \n é consumido no processo;

    ler(arquivo);

    return 0;
}



