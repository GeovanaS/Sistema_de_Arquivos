#ifndef _FAKEFS_H_
#define _FAKEFS_H_

#include "disk.h"

#define FFS_SEEK_SET 0
#define FFS_SEEK_CUR 1
#define FFS_SEEK_END 2

#define LEITURA 0
#define ESCRITA 1 
#define LEITURAESCRITA 2

struct descritor{
  struct i_node node;
  int inumber;
  int pointer_local_search;
  int offset;
  int bloco;
  int seek;
};

struct Bloco{
  void *data;
  char *text;
};

struct arquivoAbertoProcesso{
   struct arquivoAbertoTotal *aponta;
   int seek_position;
   int acesso; //modo de acesso
};

struct arquivoAbertoTotal{
   struct descritor file_table[20];
   struct arquivoAbertoProcesso abertos[20];
   int contador; //quantas vezes esse descritor foi referenciado, quando chegar a 0 posso tirar
};

struct arquivoAbertoTotal descritorArq[20];
char *blocosLivres;

int ffs_format_disk(int size, int i_size); 

int ffs_shutdown();

int ffs_create();

int ffs_open(int i_number);

int ffs_i_number(int fd);

int ffs_read(int fd, char * buffer, int size);

int ffs_write(int fd, char * buffer, int size);

int ffs_seek(int fd, int offset, int whence);

int ffs_close(int fd);

int ffs_delete(int i_number);

int ffs_check_range(int i_number);

#endif /* _FAKEFS_H_ */