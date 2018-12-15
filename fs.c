#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "disk.h"
#include "fs.h"

void inicializa_descritor(struct i_node inode, int inumber,int i,int fd){
     descritorArq->file_table[fd].node = inode;
     descritorArq->file_table[fd].inumber = inumber;
     descritorArq->file_table[fd].pointer_local_search = 0;
     descritorArq->file_table[fd].offset = 0;
     descritorArq->file_table[fd].bloco = i;
}
int getPointerLocal(){
  return descritorArq->file_table->pointer_local_search;
}

void setPointerLocal(int p){
    descritorArq->file_table->pointer_local_search = p;
}

struct i_node getInode(){
  return descritorArq->file_table->node;
}

int retornaBlocoInumber(int inumber){
  // retorna o bloco daquele inumber
  int contador = retornaQuantBlocos();
  return sBlock->size + (inumber - 1) / contador; 
}
int retornaOffset(int inumber){
  int contador = retornaQuantBlocos();
  // retorna o deslocamento do inumber dentro do inodeblock
  return (inumber - 1 ) % contador;
}

int getInumber(){
  return descritorArq->file_table->inumber;
}

int retornaPonteiroSeek(int fd){
    if(descritorArq->file_table[fd].pointer_local_search==0){
      return -1;
    }
    return getPointerLocal();
}

int retornaInumber(int fd){
  if(descritorArq->file_table[fd].inumber==0){
    return 0;
  }
  return getInumber();
}

struct i_node retornaInode(int fd){
  struct i_node nod;
  nod.size=0;
  if(descritorArq->file_table[fd].node.size==0){
    return nod;
  }
  return getInode();
}

int adicionaDescritor(struct i_node inode,int fd,int inumber, int i){
    if(descritorArq->file_table[fd].inumber != 0){
       return -1;
    }
    inicializa_descritor(inode,inumber,i,fd);
    return 0;
}


void liberaDescritor(int fd){
     descritorArq->file_table[fd].node.size = 0;
     descritorArq->file_table[fd].inumber = 0;
     descritorArq->file_table[fd].bloco = 0;
     descritorArq->file_table[fd].pointer_local_search = 0;
}

/* Retorna o proximo descritor de arquivo livre */
int retornaDescritorLivre(){
  int i;
  for(i = 0; i < MAX_FILES; ++i){
       if(descritorArq->file_table[i].inumber==0){
        return i;
       }
  }
  return -1;
}

int setSeekPtr(int fd,int ptr){
    if(descritorArq->file_table[fd].pointer_local_search==0){
      return 0;
    }else{
         retornaPonteiroSeek(ptr);
      return 1;
    }
}

void setFileSize(int fd,int size){
     descritorArq->file_table[fd].node.file_size = size;
}

void atualizaBlocoLivre(int quant,int i_size){
  //vou escrever os blocos de dados
  int i,comeco = i_size+1;
  for(i=comeco;i<quant;i++){
    blocosLivres = NULL;
    fd_write_raw(i,blocosLivres);
  }

}
int retornaPosInode(struct i_node_block *nodeB){
  int i;
   for (i = 0; i < retornaQuantBlocos(); ++i)
    {
      if(nodeB->node[i].size ==0){
        return i;
      }
    }
    return -1;
}

int verifica_arquivoAberto(int num){
 for (int i = 0; i < 20; ++i){
    if(descritorArq->file_table[i].inumber == num){
      return 1;
    }
 }
     return 0;
}



/* Inicializa o disco para o estado representando um sistema de arquivos vazio
   Ele preenche o superbloco e vincula todos os blocos de dados na lista de blocos livres.
   O parametro size representa o numero de blocos de disco no sistema de arquivos, 
   e i_size representa o numero de blocos de inode
*/


int ffs_format_disk(int size, int i_size){
  struct super_block *sBlock = malloc(sizeof(struct super_block));
  if( i_size >= size || i_size == 0 || size == 0){
     return 0;
  }
  //inicializando super bloco
  sBlock->size = size;
  sBlock->isize = i_size;
  sBlock->freeList = i_size+1; //
  fd_write_super_block(0,sBlock);
  struct i_node_block iBlock;
  int i;
  iBlock.node = NULL;
  iBlock.nNodes =0;
  for(i = 1; i <= sBlock->size; ++i){
    inicializaInode(i);
    fd_write_i_node_block(i, &iBlock);
  }
    return 1; 
}


int ffs_create(){
    struct super_block *sBlock = malloc(sizeof(struct super_block));
    struct i_node_block *inodeB = malloc(sizeof(struct i_node_block));
    struct i_node_block novo;
    struct i_node node;
    int i=0; //bloco de inode
    int j=0;

    fd_read_super_block(0,sBlock);

    for(i=1;i<sBlock->size;i++){
      fd_read_i_node_block(i, inodeB);
      if(inodeB->nNodes < retornaQuantBlocos()){
        break;
      }
      j++;
    }
    // no i eu tenho o valor do bloco
    if(j!=0)
      return -1;
   node.size = 0;
   node.flags = 0;
   node.owner = 0;
   node.file_size = 0;

   node.pointer =NULL;
   //fazer indiretos
   inodeB->nNodes++;
   int p = inodeB->nNodes -1;
   inodeB->node = (struct i_node*)realloc(inodeB->node, inodeB->nNodes*sizeof(struct i_node));
   inodeB->node[p] = node;
   
   fd_write_i_node_block(i,inodeB);
   fd_read_i_node_block(i,&novo);
   return (sBlock->size*inodeB->nNodes +1);
}




int ffs_open(int i_number){
    struct super_block *sBlock = malloc(sizeof(struct super_block));
    //struct i_node_block *inodeB = malloc(sizeof(struct i_node_block));
    struct i_node_block inodeB;

    if(!ffs_check_range(i_number)){
       return -1;
    }

    int fd = retornaDescritorLivre();
    if(fd < 0){
      return -1;
    }

    if(verifica_arquivoAberto(i_number)){
       return -1;
    }

    fd_read_super_block(0,sBlock);
    //bloco que ele está 
    int bloco = (((i_number - sBlock->isize)/ retornaQuantBlocos()))+1;
    int pos_no_inode_block = (i_number - sBlock->isize -1); 
    fd_read_i_node_block(bloco,&inodeB);

    descritorArq->file_table[fd].inumber = i_number;
    //descritorArq->file_table[fd].node = inodeB.node[pos_no_inode_block];

    descritorArq->file_table[fd].inumber = i_number;
    descritorArq->file_table[fd].pointer_local_search = 0;
    descritorArq->file_table[fd].offset = pos_no_inode_block;
    descritorArq->file_table[fd].bloco = bloco; 
    descritorArq->file_table[fd].seek = 0;

    return fd;
}


/* Libera o inode e todos os blocos do arquivo.
   Uma tentativa de excluir o arquivo que está atualmente aberto resulta em erro.
*/
int ffs_delete(int i_number){
  struct i_node_block inodeB;
    
  if(verifica_arquivoAberto(i_number)){
    return -1;
  }

  fd_read_i_node_block(i_number,&inodeB);
  inodeB.nNodes--;
  descritorArq->file_table[i_number].node.flags = 0;
  liberaBloco(i_number);
  free(descritorArq->file_table[i_number].node.pointer);
  fd_write_i_node_block(i_number,&inodeB);

  return 1;

}


/* Fecha todos os arquivos abertos e encerra o disco simulado */
int ffs_shutdown(){
  int i;
  for (i = 0; i < MAX_FILES; ++i){
        descritorArq->abertos[i].seek_position = 0;
        descritorArq->abertos[i].acesso = -1;
        descritorArq->abertos[i].aponta = NULL;
        free(blocosLivres);
        fd_stop();
        return 0;
  }
  return 0;
}

int ffs_write(int fd, char * buffer, int size){

    int bytesEscritos = 0,numEscritas = 0;
    int ptrSeek = getPointerLocal(fd);
    struct super_block *sBlock = malloc(sizeof(struct super_block));
    int num = descritorArq->file_table[fd].inumber;

    if(!verifica_arquivoAberto(num)){
       return -1;
    }

    while(bytesEscritos < size){
   
    sBlock->freeList++;
    descritorArq->file_table[fd].seek++;

    numEscritas = (abs(size - bytesEscritos));
  
    fd_write_super_block(0, sBlock);

    fd_write_raw(num,buffer); 


    bytesEscritos += numEscritas;
    ptrSeek += numEscritas;
    
    setFileSize(fd,ptrSeek); // atualiza o tamamho do arquivo
    setSeekPtr(fd,ptrSeek); // atualiza o ponteiro do seek
    descritorArq->abertos->seek_position = bytesEscritos;
    }

    return bytesEscritos;
}

int ffs_read(int fd, char * buffer, int size){
  //retorna 0 se o ponteiro de busca for maior ou igual ao tamanho do arquivo e o buffer não é modificado
  if(descritorArq->file_table[fd].seek >= descritorArq->file_table[fd].node.file_size || descritorArq->file_table[fd].node.file_size == 0){
    return 0;
  }

  if(descritorArq->abertos[fd].acesso==ESCRITA){
      return -1;
  }
  char *dado;
  int bytesLidos = 0;

  dado = (char*)calloc(get_block_size(), sizeof(char));
  int b = descritorArq->file_table[fd].inumber;
  fd_read_raw(b, dado);
  int i;
  for(i = 0; i < size; i++){
    buffer[i] = dado[i];
  }
    bytesLidos = i;
  
  descritorArq->file_table[fd].seek += size;
  descritorArq->abertos->seek_position = bytesLidos;

  return bytesLidos;
}


int ffs_i_number(int fd){
  if(fd < MAX_FILES)
    return descritorArq->file_table[fd].inumber;
  else
    return -1;

}


/* Modifica o ponteiro de busca do seek */

int ffs_seek(int fd, int offset, int whence){
  struct i_node nodo;
  nodo = descritorArq->file_table[fd].node;
  if(whence == 0){
    descritorArq->file_table[fd].seek = offset;
  }
  /*SEEK_CUR*/
  else if(whence == 1){
   descritorArq->file_table[fd].seek += offset;
  }
  /*SEEK_END*/
  else{
    descritorArq->file_table[fd].seek = nodo.file_size + offset;
  }
  return 1;
}

/*
 Escreve o inode no disco e libera a entrada da tabela de arquivo 

*/
int ffs_close(int fd){
  struct i_node_block inodeB;
  fd_read_i_node_block(descritorArq->file_table[fd].bloco, &inodeB);
  descritorArq->file_table[fd].node.flags = 0;
 // inodeB.node[file_table[fd].offset] = file_table[fd].node;
  descritorArq->file_table[fd].inumber = 0;
  descritorArq->file_table[fd].pointer_local_search = 0;
  descritorArq->file_table[fd].offset = 0;
  descritorArq->file_table[fd].bloco = 0; 
  descritorArq->file_table[fd].seek =0;
  //fd_write_i_node_block(file_table[fd].bloco, &inodeB);
  return 1; 
}


int ffs_check_range(int i_number){
  int i;
  for ( i = 0; i < MAX_FILES; ++i)
  {
    if(descritorArq->file_table[i].inumber==i_number){
      return i;
    }
  }
  return -1;
}