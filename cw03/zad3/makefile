CC = gcc
FLAGS = -Wall

clean:
	rm -Rf *.o main

test:
	$(CC) $(FLAGS) -c main.c -o main.o
	$(CC) -o main main.o
	./main "folderstart" "wzorzec" 3