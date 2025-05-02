#ifndef PGMLIB_H
#define PGMLIB_H


enum info{

    width = 0,
    heigt = 1,
    max_gr_scale = 2,

    tam_info

};

typedef struct{

    char format[4];
    int info[tam_info];

}HEADER;

typedef struct{

    int **MTZ;
    int height,width;
  
}IMG;

typedef struct{

    IMG *img;
    HEADER *header;

}ARQ;

IMG* aloc_imagem(HEADER *data);
HEADER* alloc_header();
ARQ* alloc_arq();
void desaloc_imagem(ARQ *arquivo);
void desaloc_header(ARQ *arquivo);
void desaloc_arq(ARQ **arquivo);
ARQ* ler_arquivo(char *arq_file);
void imprimir_arquivo(ARQ *arquivo, char *f_name);
ARQ* empity_template(int WIDTH, int HEIGHT, int MAX_GR_SCALE);


#endif