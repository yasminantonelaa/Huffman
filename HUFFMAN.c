#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define tam 256 // combinaçẽs possíveis de bytes

typedef unsigned char u_char; // usigned char 

typedef struct{ // dados do nó da arvore do huffman

    u_char byte; // unsigned char
    long int frequency; // frequncia absoluta
   
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

                         // funções de compressão //

/**/heap *alloc_heap(int size); // aloca a heap 
/**/no_htr *alloc_no_htr(dados_no *data); // aloca um nó da aŕvore do huffman e atribui ao campo data da arvoré a struct com os dados do nó
/**/frequency_array *alloc_frequency_array(); // aloca o array de frequecia, que é parecido com uma hash table onde a a key para acessar os indices do array são o valores convertidos para inteiro dos bytes lidos
/**/dados_no* alloc_dados_no(u_char byte); // aloca memoria para a struct que armazena os dados do nó
/**/void swap(void **a, void **b); // função de troca para uso na heap
/**/int get_parent_index(heap *hp, int i); // pega o indece pai na heap
/**/int get_left_index(heap *hp, int i); // pega o indice a esquerda na heap
/**/int get_right_index(heap *hp, int i); // pega o indice a direita na heap
/**/void min_heapfy(heap *hp, int i); // reorganiza a heap quando um valor é retirado
/**/no_htr* dequeue(heap *hp); // desenfileira um nó da Arvore
/**/void enqueue(heap *hp, no_htr *item); // aqui temos uma heap alocando um array em que cada posição tem um nó da arvore e dentro desse no tem as informações do nó
/**/void desalloc_heap(heap **hp); // desaloca a heap
/**/void read_compress(char arq[], heap **hp); // passa a heeap que será usada como fila de prioridade no huffman
/**/no_htr* create_htr(heap *hp); // cria a arvoré do huffman
/**/int get_tree_size(no_htr *root);// retona o tamnho da Arvore prevendo os caracteres de escape
/**/void generate_map(no_htr *root, u_char *path, u_char *map[]); // gera a dicionario de bits da àrvore do huffman
/**/void desaloc_map(u_char *map[]); // desaloca a memoria das strings do dicionário
/**/u_char set_bit(u_char c, int i); // gera os bits apartir da posição e do conteudo das strings
/**/int convert(char *curr_file, char *converted, u_char *map[], int tree_size); // cria o arquivo compactado e retorna o tras do cabeçalho
/**/void write_tree(FILE *fp, no_htr *root); // escreve a àrvore no cabeçalho
/**/void desaloc_tree(no_htr *root); // deasaloca a aŕvore
/**/void write_header(char *compacted_arq, no_htr *root, int trash, int tree_size); // preenche o cabeçalho
/**/void compress(); // compacta
/**/void descompress(); // descompacta
/**/int init_stack(u_char *stack, int size); // incializa stacks para funções do cabeçalho

                              // funções de descopressão //

int is_bit_set(u_char byte, int i); // verifica se o bit na posição i existe
void read_header(FILE *fp, int *trash_size, int *tree_size);  // le o cabeçalho
u_char* create_buffer(FILE *fp, int tree_size);  // aloca um buffer com o tamnho dos bytes da aŕvore
no_htr* read_bild_tree(u_char *buffer, int *locate, int tree_size);// função que lê a árvore recursivamente
void write_uncompressed_file(FILE *fp, no_htr *root, int trash, int tree_size, const char *output_filename); // função que escreve o arquivo decompactado

int main()
{
    while(1) {

        int opcao;
    
        printf("Escolha uma opção:\n");
        printf("1 - compactar arquivo\n");
        printf("2 - Descompactar arquivo\n");
        printf("3 - Encerrar\n");
        scanf("%d", &opcao);
        getchar(); // Limpa o buffer do ENTER


        switch(opcao) {
            case 1:
                compress(); 
                break;
                
            case 2:
                descompress();
                break;
    
            default:
                break;
        }

        if(opcao != 1 && opcao != 2)
        {
            break;
        }
   }

    return 0;
}

/////////////////////////////////// Função Principal Da Compactação e Descompactação ///////////////////////////////////

void compress() // compactação
{
    char arquivo[100]; // arquivo lido
    char compact_file[100]; // novo arquivo compactado

    printf("Digite o nome do arquivo a ser lido e de enter:\n");

    scanf("%99[^\n]", arquivo); // ler 99 carecteres até que seja digitado um \n, não sei se o \n é consumido no processo;
    getchar(); // consome o \n

    printf("Digite o nome do arquivo compactado e de enter:\n");
    scanf("%99[^\n]", compact_file); // le o arquivo
    getchar(); // consome o \n


    heap *hp = NULL; // incializa a heap

    read_compress(arquivo, &hp); // cria uma fila de prioridade implementada com heap para construir a árvore

    no_htr *root = create_htr(hp); // cria a aŕvore

    desalloc_heap(&hp); // desaloca a heap vazia

    int tree_size = get_tree_size(root); // pega o tamnho da árvore
    
    u_char *map[tam] = {0}; // array de ponteiros para vetor que será usado para armezenar os caminhos que traduzem a árvore

    u_char empity_path[1] = ""; // inicializa o array do caminho

    generate_map(root,empity_path,map); // cria o dicionário apartir da árvore do huffman

    int trash = convert(arquivo, compact_file, map, tree_size); // escreve o arquivo compactado e retorna valor do trash

    desaloc_map(map); // libera a memória do map

    write_header(compact_file,root,trash, tree_size); // adiciona o cabeçalho no início do arquivo compactado

    desaloc_tree(root); //desaloca a aŕvore
}

void descompress() // descompactação
{
    char arquivo[100]; // entrada compactada
    char descompact_file[100]; // saída descompactada

    printf("Digite o nome do arquivo a ser lido e de enter:\n");
    scanf("%99[^\n]", arquivo);
    getchar();

    printf("Digite o nome do arquivo descompactado e de enter:\n");
    scanf("%99[^\n]", descompact_file);
    getchar();

    FILE *fp = fopen(arquivo, "rb");
    if (!fp) {
        printf("ERRO LEITURA! VERIFIQUE O NOME DO ARQUIVO\n");
        exit(1);
    }

    int trash = 0, tree_size = 0, locate = 0;

    read_header(fp, &trash, &tree_size);

    u_char *buffer = create_buffer(fp, tree_size);
    no_htr *root = read_bild_tree(buffer, &locate, tree_size);
    free(buffer);

    write_uncompressed_file(fp, root, trash, tree_size, descompact_file);

    desaloc_tree(root);
    fclose(fp);

}

/////////////////////// Funções de Leitura Compressão /////////////////////////////

void read_compress(char arq[], heap **hp) // passa a heeap que será usada como fila de prioridade no huffman
{

    u_char byte; 
    
    FILE *file = fopen(arq, "rb"); // abre o arquivo em formato binario

    if (!file) // se o ponteiro para file for NULO
    {

        printf("ERRO LEITURA\nVERIFIQUE O NOME DO ARQUIVO\n");
        exit(1); // encerra o programa // pode retonar false e continuar rodado o menu
    }

    int arr_size = 0;  // verifica qual o tamnho do array de bytes que será mandado para heap em caso de arquivos pequenos

    frequency_array *frequency = alloc_frequency_array(); // aloca a "HASH TABLE" para fazer a contagem dos bytes

    while (fread(&byte, sizeof(u_char), 1, file) == 1) // a função de leitura enquanto bem sucedida retona os valores lidos
    {

        if (!frequency->status[(int)byte]) // verifica se os dados da posição da "Hash Table" ja exitem, se não exitirm retorna false
        {
            dados_no *no_data = alloc_dados_no(byte); // os dados serão armazenados nessa hash table atravez de um struct para os dados dos nós

            frequency->arr_bytes[(int)byte] = no_data; // essa struct é passada para "hash table" onde serão acessados e modificados por la
            frequency->status[(int)byte] = true; // marca true indicando que no indice ja foi alocado os dados

            arr_size++; // incremata o tamanho das posições validas do array de frequencia
        }

        ( (dados_no *)frequency->arr_bytes[(int)byte] )->frequency++; /// casting do tipo void do array de frequencia para o tipo struct dados no
    }

    fclose(file); // fecha o arquivo pois se encerrou a leitura

    *hp = alloc_heap(arr_size); // aloca a heap que foi declara na main

    for (int i = 0; i < tam; i++){ // dados dos nós terão que ser passados do array de frequencia para os nós explicitamente e depois colocados na heap, e importante que todo o array de frequencia seja percorrido

        if (frequency->status[i]){ // se tiver dados nessa posição da Hash Table, eles serão passados para heap

           
            no_htr *temp_node = alloc_no_htr((dados_no *)frequency->arr_bytes[i]); // passa os dados da arvore do array da hash table para os nós da arvore
            enqueue(*hp, temp_node); // enfileira os nós da àrvore na heap 
            frequency->arr_bytes[i] = NULL; // deixa de apontar para os dados que foram passados para os nós das arvores para poder ser desalocados
        }
    }

    free(frequency); // desaloca o array incial usado para a leitura

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

/////////////////////// Funções da Heap  //////////////////////////////////

heap* alloc_heap(int size) // aloca a heap
{
    heap *new = malloc(sizeof(heap)); // aloca memoria para heap
    new->data = malloc((size + 1) * sizeof(no_htr *)); // lembrando que a heap começa em 1,, deve ser desalocado
    new->max_heap_size = size + 1; // o array da heap vai até mas heap size
    new->size = 0; // inicia tamnho da heap com 0

    return new; // retona o edereço da alocação
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

no_htr* dequeue(heap *hp) // desenfileira um nó da Àrvore
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

////////////////////////////////// Funções da Arvore ////////////////////////////////////////////

no_htr *alloc_no_htr(dados_no *data) // aloca um nó da aŕvore do huffman e atribui ao campo data da arvoré a struct com os dados do nó
{
    no_htr *new = malloc(sizeof(no_htr)); // aloca a aŕvore

    new->data = data; // passa a struct com os dados da árvore
    new->left = NULL; // incializa os pontiros para os filhos
    new->right = NULL;

    return new; // retona o endereço da alocação
}
no_htr* create_htr(heap *hp) // cria a arvoré do huffman
{
    while (hp->size > 1){ // gerá a arvore até que so sobre o pai na fila
    
        no_htr *current_left = dequeue(hp); // pega o nó atual (filho) da direita
        no_htr *current_right = dequeue(hp); // pega o nó atual (filho) da esquerda

        dados_no *father_data = alloc_dados_no('*'); // atribui como caracter nulo o '*'
        father_data->frequency = ((dados_no*)current_left->data)->frequency + ((dados_no*)current_right->data)->frequency; // no pai com a soma dos filhos
        
        no_htr *father = alloc_no_htr(father_data); // cria o no pai com os dados do nó pai

        father->left = current_left; // atribui o filho da direita ao pai
        father->right = current_right; // atribui o filho da esquerda ao pai

        enqueue(hp, father); //retorna o pai para a fila de prioridades
    }

    no_htr *root = dequeue(hp); //cria a raiz da árvore

    return root; // retorna a raiz
 
}

int get_tree_size(no_htr *root) {  // retona o tamnho da arvore prevendo os carateres de escape

    if (!root){

        return 0;
    }
     
    int left_size = get_tree_size(root->left);
    int right_size = get_tree_size(root->right);

    u_char byte = ((dados_no *)root->data)->byte;

    // Se for folha e byte for '*' ou '\\', dois bytes serão usados
    if (!root->left && !root->right && (byte == '*' || byte == '\\')) {
        return left_size + right_size + 2;
    }

    // Caso normal: 1 byte
    return left_size + right_size + 1;
}

void desaloc_tree(no_htr *root) // deasaloca a aŕvore
{
    if(!root){

        return;
    }

    desaloc_tree(root->left);
    desaloc_tree(root->right);

    free(root->data);
    free(root);

    root = NULL;
}

///////////////////// Funções da Criação do Dicionário do Huffman ///////////////////

void generate_map(no_htr *root, u_char *path, u_char *map[]) // gera a dicionario de bits da àrvore do huffman
{
    if(!root){

        return;
    }

    u_char current_byte = ((dados_no*)root->data)->byte; // byte atual correspondente ao nó da aŕvore

    if(!root->left && !root->right){ // quando chega em uma folha

        map[current_byte] = strdup(path); // copia o caminho para a posição da arvore
        // atenção strdup cria um malloc que alloca dinamicamente a string no vetor portando deve ser dado free
    }

    u_char left_path[strlen(path) + 2]; // adiciona mais duas posições ao array (uma para o bit e outra para o '\0')
    u_char right_path[strlen(path) + 2]; // mesmo caso de cima

    sprintf(left_path, "%s0", path); // adiciona 0 a string da esquerda
    sprintf(right_path, "%s1", path); // adiciona 1 a string da dirita

    generate_map(root->left, left_path, map); // funções de chamada recursiva de pre-ordem
    generate_map(root->right, right_path, map); 
   
}

void desaloc_map(u_char *map[])
{
    for(int i = 0; i < tam; i++){

        if(map[i]){

            free(map[i]); //libera o que foi alocado diamicamnete com a função strdup 
        }
    }
}

//////////////////////// Funções de Escrita no Arquivo Compactado ////////////////////////

u_char set_bit(u_char c, int i) // gera os bits apartir da posição e do conteudo das strings
{
    u_char mask = 1 << (7 - i); // pois ele lê as palavras invertidas (na leitura primeiro ele lé o bit mais siguinificativo)

    return mask | c;
}

int convert(char *curr_file, char *converted, u_char *map[], int tree_size) // cria o arquivo compactado e retorna o tras do cabeçalho
{
    FILE *current_arq = fopen(curr_file,"rb"); // reabre o arquivo para fazer as leituras

    if(!current_arq){ // veribicação de abertura bem sucedida

        printf("ERRO DE RELEITURA\n");
        exit(1);
    }

    FILE *compacted_arq = fopen(converted,"wb");

    if(!compacted_arq){ // verificação de criação do arquivo bem sucedida

        printf("ERRO DE CRIAÇÃO DE ARQUIVO\n");
        exit(1);
    }

    for(int i = 0; i < (tree_size + 2); i++){ // são 2 bites + tamnho da ávore

        fputc(0, compacted_arq); // aloca o espaço vazio no cabeçalho que será sobrescrito na função de adicionar cabeçalho
    }

    u_char curr_byte; // byte a ser lido do arquivo

    u_char out_byte = 0; // byte a ser construido

    int bit_counter = 0; // maraca a posição a ser escrita bem como quantos bits ja forma escritos
    
    while (fread(&curr_byte, sizeof(u_char),1, current_arq) == 1){

        u_char *path = map[(int)curr_byte]; // aramzena a string do caminho atual

        for(int i = 0; path[i] != '\0'; i++){ // percorre a string caminho 

            if(path[i] == '1'){

                out_byte = set_bit(out_byte, bit_counter);
            }

            bit_counter++; // incrementa o contador de bits

            if(bit_counter == 8){

                fwrite(&out_byte,sizeof(u_char),1,compacted_arq); // escreve no arquivo binário

                bit_counter = 0; // reseta o contador
                out_byte = 0; // reseta a palavra
            }
        }
    }

    int trash = 0; // é a sobra do ultimo byte

    if (bit_counter > 0) {

        fwrite(&out_byte, sizeof(u_char), 1, compacted_arq);
        trash = 8 - bit_counter;
    }

    fclose(current_arq); // fecha os arquivos
    fclose(compacted_arq);

    return trash; // tamnho dos bits que sobraram
}

void write_tree(FILE *fp, no_htr *root) { // escreve a aŕvore no cabeçalho
    
    if (!root){

        return;
    }
     
    u_char byte = ((dados_no *)root->data)->byte;

    if (!root->left && !root->right){ // verifica se é uma folha

        if (byte == '*' || byte == '\\') {

            fputc('\\', fp);     // escreve o caractere de escape
        }

        fputc(byte, fp);  // escreve o byte real

    } else {

        fputc(byte, fp);  // escreve o byte do nó interno 
        
        write_tree(fp, root->left);   
        write_tree(fp, root->right);  
    }
}

int init_stack(u_char *stack, int size) // incializa stacks para funções do cabeçalho
{
    for(int i = 0; i < size - 1; i++){

        stack[i] = '0'; // inicializa com 0;
    }

    stack[size - 1] = 0; // coloca o carater nulo no final
}

void write_header(char *compacted_arq, no_htr *root, int trash, int tree_size) // gera o cabeçalho 
{
    u_char stack[17];

    init_stack(stack, 17); // atribui o caracter nulo e preenche o array com 0
 
    int stack_size_inf = 2; // 3 primeiras posições do cabelhos, bits for trash
    int stack_size_sup = 15; // 13 ultimas posições do cabeçalho bits
    
    
    while(trash > 0){ // representa os 3 primeiros bits do cabeçalho
 
        stack[stack_size_inf--] = (trash % 2 == 0) ? '0' : '1'; // preenche os do lixo bits do menos siguificativo para o mais siguinificativo
        trash /= 2;
    }
 
    while(tree_size > 0){ // representa os 13 ultimos bits do cabeçalho
 
        stack[stack_size_sup--] = (tree_size % 2 == 0) ? '0' : '1'; // preenche os bits da arvoré na mesma lógica
        tree_size /= 2;
    }

    u_char byte1 = 0, byte2 = 0;

    for (int i = 0; i < 8; i++){ // preeche os dois bytes da string simutaneamente

        if (stack[i] == '1'){ // preeche a parte do trash 3 bits + 5 bits mais siguinificativos do tamanho da aŕvore (primeiro byte)

            byte1 = set_bit(byte1, i);
        }

        if (stack[i + 8] == '1'){ // preenche o resto dos bits do tamnho da árvore ( segundo byte)

            byte2 = set_bit(byte2, i);
        } 
     }

    FILE *fp = fopen(compacted_arq, "rb+"); // leitura + escrita binária

    if (!fp) {

        printf("Erro ao reabrir o arquivo para escrever o cabeçalho.\n");
        exit(1);
    }

    fputc(byte1, fp); // sobre-escreve os bytes lidos acima
    fputc(byte2, fp);
 
    write_tree(fp, root);

    fclose(fp);
}

//////////// Funções GERAIS de descompressão ////////////

int is_bit_set(u_char byte, int i) // verifica se o bit na posição i existe
{
    u_char mask = 1 << (7 - i); // le do byte menos significativo ao mais significativo

    return mask & byte; // faz um ando lógico que retorna 0 ou 1 dependendo se há ou não bit na posição
}

void read_header(FILE *fp, int *trash_size, int *tree_size) // le o cabeçalho
{ 

    u_char byte1 = fgetc(fp); // 2 bites para trash e tree size
    u_char byte2 = fgetc(fp);

    *trash_size = byte1 >> 5; // move 5 bites para a direita => ou seja deixa so os 3 bits do trash
    *tree_size = ((byte1 & 0x1F) << 8) | byte2; // primeiro faz um ou lógico entre o byte1 e 00011111, zerando assim..
    //.. os primeiros 3 bits ficando com 000*****, logo em seguida empura 8 bites para direita ficando 000*****00000000..
    // e por ultimo faz um ou logico com o byte 2 ficando assim 000************ onde *** é o tree-size;
}


u_char* create_buffer(FILE *fp, int tree_size) // aloca um buffer com o tamnho dos bytes da aŕvore
{ 

    u_char *buffer = malloc(tree_size * sizeof(u_char)); // aloca o vetor do buffer

    if (!buffer || fread(buffer, sizeof(u_char), tree_size, fp) != tree_size) { // if de guarda

        printf("Erro ao ler a árvore.\n");
        exit(1);
    }

    return buffer;
}

no_htr* read_bild_tree(u_char *buffer, int *locate, int tree_size) // função que lê a árvore recursivamente
{ 

    if (*locate >= tree_size){
        return NULL;
    } 

    u_char byte = buffer[(*locate)++]; // dado atual do buffer

    dados_no *data = alloc_dados_no('\0'); // cria uma struct com os dados do no e não define o caractere
    no_htr *node = alloc_no_htr(data);

    if (byte == '*') { // siguinica que não é uma folha (os bits válidos são folhas)
        // se entrou aqui não é uma folha
        ((dados_no*)node->data)->byte = '*'; // atuliza os dados do nó para o cacter deafult

        node->left = read_bild_tree(buffer, locate, tree_size); // preenche recursivamente a direita
        node->right = read_bild_tree(buffer, locate, tree_size); // preeche recursivamente a esquerda

    } else { // caiu em um caractere escapado ou em uma folha válida

        /*nota: durante a impressão da Árvore do huffman é adicionado um caracter escpado '\\' ANTES de cada * ou '\\' 
        como bytes validos, então a função irá consumir o caracter escapado (se for o caso) e logo após atribuir o caracter 
        real como folha! ;D */

        if (byte == '\\' && *locate < tree_size) {
            byte = buffer[(*locate)++];
        }
        ((dados_no*)node->data)->byte = byte; // atribui o carater a folha
    }

    return node; // retrorna o nó para atulizar o ponteiro
}


void write_uncompressed_file(FILE *fp, no_htr *root, int trash, int tree_size, const char *output_filename) // ecreve o arquivo descompatado
{

    FILE *out = fopen(output_filename, "wb"); // cria o arquivo de saida

    if (!out) { // guard
        printf("Erro ao abrir arquivo de saída.\n");
        exit(1);
    }

    no_htr *current = root; // current é a posição da arvore que será percorrida durante a recosntrução dos bytes
    u_char byte;
    int bit_index;

    //fseek(fp, 2 + tree_size, SEEK_SET); // debug purposes

    fseek(fp, 0, SEEK_END); // vai pro final do arquivo
    long file_size = ftell(fp); // pega o tamanho do arquivo total
    fseek(fp, 2 + tree_size, SEEK_SET); // volta o ponteiro file para a posição correta depois dos bytes da árvore
    long current_pos = ftell(fp); // salva a posição correta para refreência do inicio da leitura (depois do bytes da árvore)

    while (current_pos < file_size - 1) { // le byte por byte até o penultimo, pois o último contém o trash

        byte = fgetc(fp);
        current_pos++; // incia ja com o tanho da arvore + header

        for (bit_index = 0; bit_index < 8; bit_index++) {

            current = is_bit_set(byte, bit_index) ? current->right : current->left; //decide para qual lado vai 0 esquerda, 1 direita

            if (!current->left && !current->right) { // quado chega a uma folha iimprime o byte e volta para raiz

                fputc(((dados_no*)current->data)->byte, out);
                current = root;
            }
        }
    }

    // último byte, respeitando o lixo
    byte = fgetc(fp);
    for (bit_index = 0; bit_index < (8 - trash); bit_index++) { // le o ultimo byte e percorre o a arvore desconsiderando os bits trash
        // repete o processo acima, basicamente
        current = is_bit_set(byte, bit_index) ? current->right : current->left;

        if (!current->left && !current->right) {

            fputc(((dados_no*)current->data)->byte, out);
            current = root;
        }
    }

    fclose(out);
}