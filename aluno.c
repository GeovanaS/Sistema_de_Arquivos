#include "fs.h"
#include "disk.h"
#include "simplegrade.h"
#include <string.h>


void test1(){

  DESCRIBE("Modifica o numero de blocos e o tamanho do bloco");
  IF("Altero o numero de blocos");
  set_block_num(52);
  THEN("Valor deve ser modificado com sucesso");
  isEqual(get_block_num(),52,1);
  IF("Modifica o tamanho dos blocos");
  set_block_size(1024);
  THEN("tamanho deve ser modificado com sucesso");
  isEqual(get_block_size(),1024,1);

  char buffer[1024];
  memset(buffer,'b',1024); //preenche vetor com b's

  IF("Escrevo em 20 blocos");
  THEN("Conteudo lido deve corresponde ao conteudo escrito");
  fd_write_raw(20,buffer);
  fd_read_raw(20,buffer);
  int i;
  for(i=0;(i < 1024) && (buffer[i] == 'b');++i){
    if(buffer[i]!='b'){
        break;
    }
  }
  isEqual(i, 1024, 1);

  fd_stop();
}

void test2(){
  
  DESCRIBE("Formato o disco e crio um arquivo");
  int i_number,fd,i;
  char b[4] ={'a','b','c','d'};
 
  set_block_size(100);
  IF("Testa a formatação");
  THEN("Deve formatar o disco");
  isEqual(ffs_format_disk(10,5),1,1); 
  
  i_number = ffs_create();
  fd = ffs_open(i_number);

  IF("Escrevo em 4 blocos do arquivo");
  for(i = 0; i < 4; ++i){
    ffs_write(fd,b,4);
  }

  ffs_seek(fd,0,0);
  THEN("Deve ler os 4 blocos escritos");
  isEqual(ffs_read(fd,b,4),4,1);

  ffs_close(fd);

  fd_stop();
}

void test3(){
  char buf[150];
  int i_number,fd[2],i;

  DESCRIBE("Teste com dois arquivos");

  memset(buf,'c',150); //preenche vetor com b's

  set_block_size(150);
  ffs_format_disk(50,1); 

  i_number = ffs_create();
  
  fd[0] = ffs_open(i_number);
  
  IF("Escrevo no primeiro arquivo");
  THEN("Deve escrever um bloco");
  ffs_write(fd[0],buf,150);
  ffs_seek(fd[0],0,0);
  ffs_read(fd[0],buf,150);
 
  int flag=0;
  for (i = 0; i < 150; ++i){
      if(buf[i]=='c')
         flag = 1;
      else
         flag = 0;
  }
  THEN("Deve ler o mesmo conteudo escrito");
  isEqual(flag,1,1);

  ffs_close(fd[0]);
  ffs_delete(fd[0]);
  
  char buffer[1]={'a'};

  fd[1] = ffs_open(i_number);

  IF("Escrevo um bloco para o segundo arquivo");
  THEN("Deve escrever um bloco");
  ffs_write(fd[1],buffer,1);
  ffs_seek(fd[1],0,0);
  if(buffer[0]=='a'){
     isEqual(ffs_read(fd[1],buffer,1),1,1);
  }else{
    isEqual(ffs_read(fd[1],buffer,1),0,1);
  }
  ffs_close(fd[1]);
  ffs_delete(fd[1]);

  fd_stop();
}



int main(){

    DESCRIBE("Testes do trabalho 2: Sistema de Arquivos");
   
    test1();
    test2();
    test3();


    GRADEME();
    if(grade == maxgrade)
      return 0;
    else
      return grade;
}