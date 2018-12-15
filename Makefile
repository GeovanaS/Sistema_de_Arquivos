CC=gcc

CFLAGS=-std=c11 -lm -Wall -Wextra -Werror
LDFLAGS=-lm

all: run_aluno grade 

disk.o: disk.c

fs.o: fs.c	

aluno.o: aluno.c

test.o: test.c

aluno: disk.o fs.o aluno.o 
	$(CC)  $(LDFLAGS) disk.o fs.o aluno.o -lm -o aluno

test: disk.o fs.o test.o
	$(CC) $(CFLAGS) $(LDFLAGS) disk.o fs.o test.o -lm -o test

run_aluno: aluno
	./aluno

grade: test
	./test

clean:
	rm -rf *.o aluno test 
