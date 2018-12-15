#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "disk.h"
#include "fs.h"

int BLOCK_SIZE = 512;
int BLOCK_NUM = 256;
int contR=0,contW=0;
FILE *disk;
struct Bloco *bloco = NULL;
struct arquivoAbertoTotal *arqtotal;

struct i_node_block *inicializa_i_node_block();
struct indirect_block *inicializa_indirect_block();
struct super_block *inicializa_superblock();
void stop(int removeArquivo);
int retornaNumEscritas();
int retornaNumLeituras();
//struct i_node_block *pega_i_node_block_livre();
int get_num_block_indirect();


void set_block_size(int block_size){
  BLOCK_SIZE = block_size;
}

int get_block_size(){
  return BLOCK_SIZE;
}
int get_num_block_indirect(){
  int num;
  num = BLOCK_SIZE/4;
  return num;
}
void set_block_num(int block_num){
  BLOCK_NUM = block_num;
  if(bloco == NULL){
    bloco = realloc(bloco,block_num*sizeof(struct Bloco));
  }
}

int get_block_num(){
  return BLOCK_NUM;
}

int retornaQuantBlocos(){
  return (get_block_size() / 64);
}

void liberaBloco(int inumber){
  int i;
  for(i = 0; i < get_block_size(); ++i){
     bloco[inumber].text[i] = '\0';
  }
}


// Função que inicializa as variaveis da estrutura i_node 


struct i_node_block *inicializa_i_node_block(){
  struct i_node_block *inodeB = malloc(sizeof(struct i_node_block));
  if(inodeB==NULL){
    assert(inodeB);
  }
  
  inodeB->nNodes = 0;
  int i,tam;
  tam =  get_block_size()/I_NODE_SIZE;
  for(i = 0; i < tam; i++){
      inodeB[i].node = malloc(sizeof(struct i_node_block)*tam);   
      assert(inodeB[i].node);
  }

  return inodeB;
}


// Função que inicializa as variaveis da estrutura super_block OK
struct super_block *inicializa_superblock(){
   struct super_block *sBlock = malloc(sizeof(struct super_block));

   if(sBlock==NULL){
       assert(sBlock);
   }
   sBlock->size = 0;
   sBlock->freeList = 0;
   sBlock->isize = 0;

   return sBlock;          
}

void inicializaInode(int block_num){
  bloco[block_num].data = malloc(sizeof(struct i_node_block));
}


void fd_read_raw(int block_num, char * buffer){
    int i;

    if(block_num < 0 || block_num >= BLOCK_NUM || buffer == NULL){
        return;
        //exit(1); //argumentos invalidos
    }else{
         for(i = 0; i < BLOCK_SIZE; i++){
            buffer[i] = bloco[block_num].text[i];
         }
    }

    contR++; //incrementa o numero de leituras realizadas
}
void fd_write_raw(int block_num, char * buffer){
      bloco[block_num].text  = (char*) calloc(strlen(buffer), sizeof(char));
      int i;

    if(block_num < 0 || block_num >= BLOCK_NUM){
        return;
        //exit(1); //argumentos invalidos
    }else{
      for(i = 0; i < BLOCK_SIZE; i++){
        bloco[block_num].text[i] = buffer[i];
      }
        bloco[block_num].data = (void*)buffer;
    }
    contW++; //incrementa o numero de escritas 
}


void fd_read_super_block(int block_num, struct super_block * buffer){    

    if(block_num < 0 || block_num >= BLOCK_NUM || buffer == NULL){
        return;
    }else{
      buffer =  (struct super_block*)bloco[block_num].data;
     // bloco = bloco + (BLOCK_SIZE*block_num);
      //buffer = (struct super_block*)bloco 
    }

    contR++; //incrementa o numero de leituras realizadas
}

void fd_write_super_block(int block_num, struct super_block * buffer){
    if(block_num < 0 || block_num >= BLOCK_NUM){
       return;
    }else{
      bloco[block_num].data = buffer;
    }
    contW++;  //incrementa o numero de escritas realizadas
}


void fd_read_i_node_block(int block_num, struct i_node_block * buffer){
     if(block_num < 0 || block_num >= BLOCK_NUM || buffer == NULL){
        exit(1); //argumentos invalidos
    }else{
      buffer = (struct i_node_block *) bloco[block_num].data;
    }
    contR++; //incrementa o numero de leituras realizadas
}

void fd_write_i_node_block(int block_num, struct i_node_block * buffer){
    if(block_num < 0 || block_num >= BLOCK_NUM || buffer == NULL){
        return;
    }else{
      bloco[block_num].data = buffer;
  }
    contW++;   //incrementa o numero de escritas realizadas
}

void fd_read_indirect_block(int block_num, struct indirect_block * buffer){
    if(block_num < 0 || block_num >= BLOCK_NUM || buffer == NULL){
        exit(1); //argumentos invalidos
    }else{
        buffer = (struct indirect_block *) bloco[block_num].data;
   }
    
    contR++; //incrementa o numero de leituras realizadas
}

void fd_write_indirect_block(int block_num, struct indirect_block * buffer){    
    if(block_num < 0 || block_num >= BLOCK_NUM){
        exit(1); //argumentos invalidos
    }else{
      bloco[block_num].data = buffer;
  }

    contW++; //incrementa o numero de escritas realizadas
}


/* Função auxiliar que retorna o numero de leituras realizadas*/
int retornaNumLeituras(){
    return contR;
}

/* Função auxiliar que retorna o numero de escritas realizadas*/
int retornaNumEscritas(){
    return contW;
} 


/* Função auxiliar para a remoção do arquivo utilizada na função fd_stop */
void stop(int removeArquivo){ 
    if(removeArquivo==1){
       remove("DISK");
    }
}

int fd_stop(){
  stop(1);
  return retornaNumLeituras() && retornaNumEscritas();
}