#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define tam 256 // combinaçẽs possíveis de bytes

typedef unsigned char u_char; // usigned char 

typedef struct{ // dados do nó da arvore do huffman

    u_char byte;            // unsigned char
    long int frequency;          // frequncia absoluta
    long double frequency_prcnt; // frequncia relativa em porcentagem

} dados_no;

typedef struct no_htr{ // nó da arvore do huffman

    void *data;           // struct com dados do nó
    struct no_htr *left;  // filho da direita
    struct no_htr *right; // filho da esquerda

} no_htr; // no_htr = no huffman tree

typedef struct{ // struct que gurada o array que marca a frequncia dos bytes funciona como uma hash table adaptada onde as chaves serão os bytes

    void *arr_bytes[tam]; // arrai que guarda os dados do nó
    bool status[tam];     // true se exite e false se não existe;

}frequency_array;

typedef struct{ // struct da heap a unica diferença pratica entre heap min e heap max são os comparadores < e > nas verificações

    int size, max_heap_size; // tamnho da heap e tamnho máximo da heap
    void **data; // nós da árvore

}heap;

heap *alloc_heap(int size) // aloca a heap
{
    heap *new = malloc(sizeof(heap)); // aloca memoria para heap
    new->data = malloc((size + 1) * sizeof(no_htr *)); // lembrando que a heap começa em 1,, deve ser desalocado
    new->max_heap_size = size + 1; // o array da heap vai até mas heap size
    new->size = 0; // inicia tamnho da heap com 0

    return new; // retona o edereço da alocação
}

no_htr *alloc_no_htr(dados_no *data) // aloca um nó da aŕvore do huffman e atribui ao campo data da arvoré a struct com os dados do nó
{
    no_htr *new = malloc(sizeof(no_htr)); // aloca a aŕvore

    new->data = data; // passa a struct com os dados da árvore
    new->left = NULL; // incializa os pontiros para os filhos
    new->right = NULL;

    return new; // retona o endereço da alocação
}

frequency_array *alloc_frequency_array() // aloca o array de frequecia, que é parecido com uma hash table onde a a key para acessar os indices do array são o valores convertidos para inteiro dos bytes lidos
{
    frequency_array *new = malloc(sizeof(frequency_array));

    for (int i = 0; i < tam; i++)
    {
        new->arr_bytes[i] = NULL; // array onde cada posição irá apontar para uma struct dos dados do nó
        new->status[i] = false; // arry que ira dizer se na posição acessada pela chave(os bytes lidos) ja foi alocado a struct
    }

    return new; // retorna o edereço alocado
}

dados_no* alloc_dados_no(u_char byte) // aloca memoria para a struct que armazena os dados do nó
{
    dados_no *new = malloc(sizeof(dados_no)); // aloca a struct dos dados do nó
    new->byte = byte; // atribui a struct o valor do byte
    new->frequency = 0; // inicializa a frequência

    return new; // retorna o endereço alocado
}

void swap(void **a, void **b) // função de troca para uso na heap,
{
    void *z; // variável axiliar para fazer a troca

    z = *a; // valor de z guarda o valor de a
    *a = *b; // valor de a recebe o valor da troca que é o valor de a e o valor de a se perde
    *b = z; // valor de b recebe o valor de a que estava guardado no valor de z
}

int get_parent_index(heap *hp, int i) // pega o indece pai na heap
{
    return i >> 1; // i/2
}

int get_left_index(heap *hp, int i) // pega o indice a esquerda na heap
{
    return i << 1; // i * 2
}

int get_right_index(heap *hp, int i) // pega o indice a direita na heap
{

    return (i << 1) + 1; // i * 2 + 1
}

void min_heapfy(heap *hp, int i) // reorganiza a heap quando um valor é retirado
{
    int smallest; // armazanear o indice do menor valor
    int left_index = get_left_index(hp, i); // guarda o indice do ""no"" a esquerda (heap é uma simulação de arvore binaria)
    int right_index = get_right_index(hp, i); // guarda o indice do ""no""" a direita

    if (left_index <= hp->size &&
      ((dados_no*)((no_htr*) hp->data[left_index])->data)->frequency  <  ((dados_no*)((no_htr*)hp->data[i])->data)->frequency){ // como as structs são do tipo void deve-se fazer os castings constantes para os tipos requeridos
// esse if verifica se o filho da dirita é menor que o pai e se for o menor valor será o filho da esquerda
        smallest = left_index;
    } else {

        smallest = i; // se o filho da esquerda não for o menor valor ele coloca o menor valor no indice passado
    }

    if (right_index <= hp->size &&
        ((dados_no*)((no_htr*)hp->data[right_index])->data)->frequency < ((dados_no*) ((no_htr*)hp->data[smallest])->data)->frequency){ // casting de ponteiro para void da heap para o tipo nó da arvore e do tipo data ponteiro ponetiro para void da arvore para struct dados do no

        smallest = right_index; // verifica se o filho da esquerada é menor que o da direita, se for o menor é o filho da dirira
    }

    if (smallest != i){ // quando o menor é igual a raiz (primeira posição do array) as propriedades da heap estão atendidas
    
        swap(&hp->data[i], &hp->data[smallest]); // ele faz as trocas tenatndo colocar o menor elemento no indice i
        min_heapfy(hp, smallest); // chama a função recusivamente até que as propriedades da heap seja satisfeitas
    }
}

no_htr *dequeue(heap *hp)
{

    if (hp->size == 0){ // heap está vazia

        return NULL;
    }

    no_htr *min = hp->data[1]; // a primeira posição da heap minima sempre é o menor elemento

    hp->data[1] = hp->data[hp->size]; // aqui ele pega o ultimo elemento e coloca na primeira posição para recontruir a heap
    hp->size--; // aqui ele faz uma remoção LOGICA da ultima posição da heap

    min_heapfy(hp, 1); // reconstroi a heap

    return min; // retona o elemnto retirado
}

void enqueue(heap *hp, no_htr *item) // aqui temos uma heap alocando um array em que cada posição tem um nó da arvore e dentro desse no tem as informações do nó
{
    if (hp->size >= hp->max_heap_size)
    {
        printf("Heap Overflow");
        return;
    }

    hp->data[++hp->size] = item; // item que será enfilieirado , note que ha o incremento da posição antes de infileirar, então começará de do indice 1

    int key_index = hp->size; // pega o idice atual
    int parent_index = get_parent_index(hp, hp->size); // pega o indice do pai

    while (parent_index > 0 &&
        ((dados_no*)((no_htr*)hp->data[key_index])->data)->frequency  <  ((dados_no*)((no_htr*)hp->data[parent_index])->data)->frequency) {

        swap(&hp->data[key_index], &hp->data[parent_index]); // a troca é feita entre o filho e o pai, quando o pai é meor que o filho até satisfazer as propiedades da heap min

        key_index = parent_index; // o inde chave passa a ser o indice do pai para continuar verificando se os "nos" (posições do array) acima satisfazem a propriedade

        parent_index = get_parent_index(hp, key_index); // pega o nó de pai
    }
}

void desalloc_heap(heap **hp) // desaloca a heap
{
    free((*hp)->data); // dealoca o array dinamico da heap
    free(*hp); // dealoca o malloc da heap

    *hp = NULL; // ponteiro pra heap aponta para null (boas praticas);
}

void ler(char arq[], heap **hp) // passa a heeap que será usada como fila de prioridade no huffman
{

    u_char c; // usar unsigned char para ler pq o char simples terar um bit ou byte, sla, para o final
    // provavelmente deve-se declarar a heap na main para contruir a aŕvore

    FILE *file = fopen(arq, "rb"); // abre o arquivo em formato binario

    if (!file) // se o ponteiro para file for NULO
    {

        printf("ERRO LEITURA\nVERIFIQUE O NOME DO ARQUIVO\n");
        exit(1); // encerra o programa // pode retonar false e continuar rodado o menu
    }

    long int qtd_bytes = 0; // verifica o tamnho do aquivo em bytes
    int arr_size = 0;  // verifica qual o tamnho do array de bytes que será mandado para heap em caso de arquivos pequenos

    frequency_array *frequency = alloc_frequency_array(); // aloca a "HASH TABLE" para fazer a contagem dos bytes

    while (fread(&c, sizeof(u_char), 1, file) == 1) // a função de leitura enquanto bem sucedida retona os valores lidos
    {

        if (!frequency->status[(int)c]) // verifica se os dados da posição da "Hash Table" ja exitem, se não exitirm retorna false
        {
            dados_no *no_data = alloc_dados_no(c); // os dados serão armazenados nessa hash table atravez de um struct para os dados dos nós

            frequency->arr_bytes[(int)c] = no_data; // essa struct é passada para "hash table" onde serão acessados e modificados por la
            frequency->status[(int)c] = true; // marca true indicando que no indice ja foi alocado os dados

            arr_size++; // incremata o tamanho das posições validas do array de frequencia
        }

        ( (dados_no *)frequency->arr_bytes[(int)c] )->frequency++; /// casting do tipo void do array de frequencia para o tipo struct dados no
        qtd_bytes++; // incremnta a quantidade de bytes para controle dos bytes lidos
    }

    fclose(file); // fecha o arquivo pois se encerrou a leitura

    *hp = alloc_heap(arr_size); // aloca a heap que foi declara na main

    for (int i = 0; i < tam; i++){ // dados dos nós terão que ser passados do array de frequencia para os nós explicitamente e depois colocados na heap, e importante que todo o array de frequencia seja percorrido

        if (frequency->status[i]){ // se tiver dados nessa posição da Hash Table, eles serão passados para heap

            ( (dados_no *) frequency->arr_bytes[i] )->frequency_prcnt = (( (dados_no *)frequency->arr_bytes[i])->frequency  / (long double)qtd_bytes) * 100; // castings que ocorrem por causa do ponteiro para void e casting para double para calcular a porcentage realativa dos nós
            no_htr *temp_node = alloc_no_htr((dados_no *)frequency->arr_bytes[i]); // passa os dados da arvore do array da hash table para os nós da arvore
            enqueue(*hp, temp_node); // enfileira os nós da àrvore na heap 
            frequency->arr_bytes[i] = NULL; // deixa de apontar para os dados que foram passados para os nós das arvores para poder ser desalocados
        }
    }

    free(frequency); // desaloca o array incial usado para a leitura

    printf("Tamanho do arquivo lido %ld bytes\n", qtd_bytes); // mensagem de debug
    
}

int main()
{
    char arquivo[100]; // arquivo de teste

    printf("Digite o nome do arquivo: \n");

    scanf("%99[^\n]", arquivo); // ler 99 carecteres até que seja digitado um \n, não sei se o \n é consumido no processo;


    heap *hp = NULL;

    ler(arquivo, &hp);

    ///// apartir daqui o código ja está pronto para ser usado para construir a árvore do huffman usando a função dequeue da heap
    /// daqui para baixo é so debug e desolcar a memoria

    int soma_frequency = 0;  // valor de verificação se todos os bits foram lidos
    double soma_percent = 0; // valor para verificar se as porcentagen atribuidas batem
    
    while (1)
    {
        no_htr *temp = dequeue(hp); // note que essa é a fila de prioridade que será usada na contrução da aŕvore
        // para obter os nós da arvore é so desenfileirar até que a função retorne null indicando o fim da heap

        if (!temp) // fim da heap
        {
            break;
        }

        printf("BYTE: %d, frequency %ld, percent %Lf\n", (int)((dados_no*)temp->data)->byte, ((dados_no*)temp->data)->frequency, ((dados_no*)temp->data)->frequency_prcnt ); // debug

        soma_frequency += ((dados_no*)temp->data)->frequency; // debug
        soma_percent += ((dados_no*)temp->data)->frequency_prcnt; // debug

        free(temp->data); // estou liberado mas vc irá usar esses nós na aŕvore 
        free(temp); // mesma cituação de cima

        // no processo de liberação primeiro são liberados os dados do nó e depois o nó
    }

   desalloc_heap(&hp); // desaloca o resto da heap


/// memory leak free

    printf("\n");

    printf("Soma dos Bytes da frequência %d bytes\n", soma_frequency); // debug
    printf("Soma da porcentagens da frequência %lf\n", soma_percent); // debug
    
    return 0;
}
