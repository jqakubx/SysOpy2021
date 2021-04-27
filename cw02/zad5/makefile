CC = gcc 
FLAGS = -Wall


all: static shared clean

static:
	$(CC) ${FLAGS} -c main.c -o main.o
	ar rcs main.a main.o
	$(CC) -o main main.c

static2:
	$(CC) ${FLAGS} -c main2.c -o main2.o
	ar rcs main2.a main2.o
	$(CC) -o main2 main2.c

clean:
	rm -Rf *.o *.a *.so *.out main main2

test:
	make clean
	rm -Rf pomiar_zad_5.txt output.txt file2.txt file3.txt
	make static
	./main "file1.txt" "file2.txt"
	make clean
	make static2
	./main2 "file1.txt" "file3.txt"
	make clean

	
	