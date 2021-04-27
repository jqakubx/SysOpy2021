CC = gcc 
FLAGS = -Wall


all: static shared clean

static:
	$(CC) ${FLAGS} -c lib.c -o lib.o
	ar rcs lib.a lib.o
	$(CC) -o lib lib.c

shared:
	$(CC) -fpic ${FLAGS} -c lib.c -o lib.o
	$(CC) -fpic ${FLAGS} -shared -o lib.so lib.o

clean:
	rm -Rf *.o *.a *.so *.out lib 

test:
	make static
	./lib "file1.txt:file2.txt" "file3.txt:file4.txt"