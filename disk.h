#ifndef _FAKEDISK_H_
#define _FAKEDISK_H_

#define I_NODE_SIZE 64
#define MAX_FILES 20

struct i_node{
	int size; //64
	int flags;
	int owner;
	int file_size; 
	int *pointer; // deve ser alocado na inicializacao
};


struct i_node_block{
	struct i_node *node;
	int nNodes;
};

struct indirect_block{
	 int *ptr;
};

struct super_block{
	int size;  //numero de blocos no sistema de arquivo
	int freeList; //primeiro bloco na lista livre
	int isize; //numero de blocos indices
};


struct super_block *sBlock;


/* Funcoes para mudar e obter o tamanho e numero de blocos simulados */
void set_block_size(int block_size); //default 512
int get_block_size();
void inicializaInode(int block_num);
void set_block_num(int block_num); //default 256
int get_block_num();
int retornaQuantBlocos();


void liberaBloco(int inumber);
struct i_node_block *inicializa_i_node_block();

// Como definido no livro 
void fd_read_raw(int block_num, char * buffer);

void fd_write_raw(int block_num, char * buffer);

void fd_read_super_block(int block_num, struct super_block * buffer);

void fd_write_super_block(int block_num, struct super_block * buffer);

void fd_read_i_node_block(int block_num, struct i_node_block * buffer);

void fd_write_i_node_block(int block_num, struct i_node_block * buffer);

void fd_read_indirect_block(int block_num, struct indirect_block * buffer);

void fd_write_indirect_block(int block_num, struct indirect_block * buffer);

int fd_stop();


#endif /* _FAKEDISK_H_ */
