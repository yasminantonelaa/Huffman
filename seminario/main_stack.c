#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stacklib.h"
#include "pgmlib.h"

typedef struct{ // struct para armazenar um tipo arquivo na pilha

    ARQ *image;

}stack_image;

stack_image* alloc_stack_image() // aloca memoria para estruct que armazena o tipo arquivo
{
    stack_image *new = malloc(sizeof(stack_image));
    new->image = NULL;

    return new;
}

ARQ* upsidedown_img(ARQ *atual){ // inverte a imagem em relação ao eixo x

    ARQ *new_img = empity_template(atual->header->info[width],
    atual->header->info[heigt], atual->header->info[max_gr_scale]); // cria um template de imagem vazio nas especificações dos parâmetros

    int j = 0;

    for (int i = atual->img->height - 1; i >= 0; i--) { // preenche o template vazio com os dados da imgem ja fazendo a inversão

        for (int ii = 0; ii < atual->img->width; ii++) {

            new_img->img->MTZ[j][ii] = atual->img->MTZ[i][ii];
        }

        j++;
    }

    return new_img; // retrona o novo tempalte

}

ARQ* invert_img(ARQ *atual){ // inverte a imagem em relação ao eixo y, segue as mesmas especificações da função de cima

    ARQ *new_img = empity_template(atual->header->info[width],
    atual->header->info[heigt], atual->header->info[max_gr_scale]);

    int jj;

    for (int i = 0; i < atual->img->height; i++){

        jj = 0;

        for (int ii = (atual->img->width - 1); ii >= 0; ii--) {

            new_img->img->MTZ[i][jj] = atual->img->MTZ[i][ii];
            jj++;
        }
    }

    return new_img;

}

void free_stack_no(void *data){ //função para auxiliar na liberação de memória na pilha
 
    desaloc_arq(&((stack_image*)data)->image);
    free(data);
}

int main() {

    stack *UNDO = create_stack();
    stack *REDO = create_stack();

    char comando[3];
    char arquivo[257];

    printf("Digite o nome do arquivo (.pgm) e de enter:");
    scanf("%256[^\n]", arquivo);
    getchar();

    ARQ *arq = ler_arquivo(arquivo); // retorna um ponteiro para uma variável do tipo arq com todas as informaçoes da imagem

    // assim que abrir ja coloca  uma cópia da imagem na pilha do UNDO, essa posição não poderá ser desempilhada
    stack_image *deep_copy = alloc_stack_image(); // cria um struct para empilhar o tipo arquivo
    deep_copy->image = arq; // passa o endero do arquivo para a struct

    push(UNDO, deep_copy); // empilha na pilha do UNDO
    
    while (1) {

        printf("\n--- Menu ---\n");

        printf("[1] UPSIDE_DOWN\n");
        printf("[2] ESPELHAR\n");
        printf("[3] UNDO\n");
        printf("[4] REDO\n");
        printf("[5] EXIT\n");
        printf("Escolha: ");

        fgets(comando, sizeof(comando), stdin);

        if(comando[0] == '1'){

            ARQ *new_upsidedown = upsidedown_img(arq); // chama  a função de modificação e depois faz o procedimento de armazenar na struct
            stack_image *upsidedown_node  = alloc_stack_image();
            upsidedown_node->image = new_upsidedown;
            arq = new_upsidedown; // o ponteiro arq (arquivo atual) guarda sempre o endereço da modicação para ser passado como parametro para as proximas

            push(UNDO, upsidedown_node); // a cada nova função chamada o arquivo gerado é incialmente empilhado na pilha do UNDO

            if(REDO->top){ // a cada nova chamda a pilha do REDO é esvaziada

                free_unitary_stack(REDO, free_stack_no);
            } 

            imprimir_arquivo(new_upsidedown, arquivo); // as modificações são aplicadas no arquivo

        }else if (comando[0] == '2') { // memo procedimento da parte de cima, so muda a função chamada
            
            ARQ *new_inverted = invert_img(arq);
            stack_image *inverted_node  = alloc_stack_image();
            inverted_node->image = new_inverted;
            arq = new_inverted;

            push(UNDO, inverted_node);

            if(REDO->top){

                free_unitary_stack(REDO, free_stack_no);
            }

            imprimir_arquivo(new_inverted, arquivo);

        } else if (comando[0] == '3') { // mesmo procedimento que vai ser descrito abaixo, eu não vi que tinha passado ;D !

            void *data = undo(UNDO, REDO);

            if (data){

                arq = ((stack_image*)data)->image;  
                printf("DESFEITO \n");
                imprimir_arquivo(arq, arquivo);

            }else{

                printf("EMPITY STACK!\n");
            }

        } else if (comando[0] == '4') { // desfazer

            void *data = redo(UNDO, REDO); // é chamdo o procedimento desfazer especificado na biblioteca stacklib.h

            if (data){ // o procedimento retona um ponteiro com o endereço da imagem com a ação defeita

                arq = ((stack_image*)data)->image;  // sendo o ponteiro válido o arquivo atual arq é atulizado
                printf("REFEITO!\n");
                imprimir_arquivo(arq, arquivo); // as modificações são aplicadas ao arquivo de saída

            }else{ // se for inválido imprime stack vazia

                printf("EMPITY STACK!\n");
            }

        } else if (comando[0] == '5') {

            free_stacks(UNDO, REDO, free_stack_no); // libera toda a memória alocada e destroi as stacks

            printf("Saindo...\n");
            break;

            
        } else { // default para comandos inválidos

            printf("Comando inválido!\n"); 
        }

    }

    return 0;
}

