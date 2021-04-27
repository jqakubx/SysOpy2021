CC = gcc
FLAGS = -Wall

clean:
	rm -Rf *.o *.out zad

test:
	make clean
	$(CC) $(FLAGS) -c zad.c -o zad.o
	$(CC) -o zad zad.o
	./zad 15

