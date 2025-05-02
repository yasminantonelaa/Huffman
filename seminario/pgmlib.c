#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pgmlib.h"

IMG* aloc_imagem(HEADER *data) // aloca uma matriz dinâmica para armezanr os pixels da imagem
{
    IMG *NEW_IMG = malloc(sizeof(IMG));

    NEW_IMG->MTZ = malloc(data->info[heigt] * sizeof(int *));

    for(int i = 0; i < data->info[heigt]; i++){

        NEW_IMG->MTZ[i] = malloc(data->info[width] * sizeof(int));
    }

    NEW_IMG->height = data->info[heigt]; // prenche a struct da matriz com a altura e largura
    NEW_IMG->width = data->info[width];

    return NEW_IMG;
}

HEADER* alloc_header() // aloca um cabeçalho que gurada uma string (fomat) para o formato e um array para outras informações (info)
{
    HEADER *new = malloc(sizeof(HEADER));
    return new;
}

ARQ* alloc_arq() // aloca a struct do arquivo em si, que organiza as outras duas structs
{
    ARQ *new = malloc(sizeof(ARQ));
    new->header = NULL;
    new->img = NULL;

    return new;
}

void desaloc_imagem(ARQ *arquivo) // libera a memória da struct da matriz da imagem
{
    for(int i = 0; i < arquivo->img->height; i++){

        free(arquivo->img->MTZ[i]);
    }

   free(arquivo->img->MTZ);
   free(arquivo->img);

   arquivo->img = NULL;
}

void desaloc_header(ARQ *arquivo) // libera memória da srtuct do cabeçalho do arquivo
{
    free(arquivo->header);
    arquivo->header = NULL;
}

void desaloc_arq(ARQ **arquivo) // libera memoria da struct do arquivo e destroi essa struct
{
    desaloc_imagem(*arquivo);
    desaloc_header(*arquivo);
    free(*arquivo);

    *arquivo = NULL;
}


ARQ* ler_arquivo(char *arq_file) // faz a leitura do arquivo
{
    FILE *fp = fopen(arq_file,"rt"); // abre em modo de leitura de texto

    if(!fp){

        printf("ERRO AO ABRIR ARQUIVO!\n");
        exit(1);
    }

    ARQ *arquivo = alloc_arq(); // alooca a struct geral do arquivo
    arquivo->header = alloc_header(); // aloca a struct para receber os dados do cabeçalho

    fscanf(fp,"%s",arquivo->header->format); // le o formato
    
    if(strcmp("P2",arquivo->header->format) != 0){ // verifica se o formato está correto

        printf("ARQUIVO INCOMPATÌVEL! \n");
        exit(1);
    }
    
    for(int k = 0; k < 3; k++){ // lê os dados das informações da imagem

        fscanf(fp,"%d",&arquivo->header->info[k]);
    }
    
    arquivo->img = aloc_imagem(arquivo->header); // aloca a matriz para receber os pixels da imagem

    for(int i = 0; i < arquivo->img->height; i++){ // preenche a matriz com os pixels

        for(int ii = 0; ii < arquivo->img->width; ii++){

            fscanf(fp,"%d",&arquivo->img->MTZ[i][ii]);
        }        
    }

    fclose(fp); // fecha o arquivo

    return arquivo;

}

void imprimir_arquivo(ARQ *arquivo, char *f_name) // imprime o arquivo seno passado como paramtro as informações do arquivo e uma struct com o nome do arquivo
{
    FILE *fp = fopen(f_name,"w+"); // abre em modo de escrita 

    fprintf(fp,"%s\n",arquivo->header->format); // imprime o cabeçalho
    fprintf(fp,"%d %d\n%d\n",arquivo->header->info[width],
            arquivo->header->info[heigt], arquivo->header->info[max_gr_scale]);
    
    for(int i = 0; i < arquivo->img->height; i++){ // imprime a matriz dos pixels da imagem

        for(int ii = 0; ii < arquivo->img->width; ii++){
            
                fprintf(fp,"%d ",arquivo->img->MTZ[i][ii]);

        }

        fprintf(fp,"\n");
    }

    fclose(fp); // fecha o arquivo
    
}

ARQ* empity_template(int WIDTH, int HEIGHT, int MAX_GR_SCALE) // cria um template vazio para que seja criada uma nova imagem
{
    ARQ *new_template = alloc_arq(); // cria a struct imagem com as informações gerais
    new_template->header = alloc_header(); // aloca memória para o cabeçalho

    char format[] = "P2";

    strcpy(new_template->header->format, format); // preeche o  cabeçalho com todas as informaçẽos da imagem 

    new_template->header->info[width] = WIDTH; 
    new_template->header->info[heigt] = HEIGHT;
    new_template->header->info[max_gr_scale] = MAX_GR_SCALE;

    new_template->img = aloc_imagem(new_template->header); // aloca uma matriz vazia para os pixels

    return new_template; // retorna o template

}