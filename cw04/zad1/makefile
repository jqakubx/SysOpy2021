CC = gcc -Wall 

clean:
	rm -Rf *.o *.out main exec

test:
	make clean
	$(CC) -o main main.c
	./main ignore fork
	./main pending fork
	./main mask fork
	./main handler fork
	make clean
	$(CC) -o main main.c
	$(CC) -o exec exectest.c
	./main ignore exec
	./main pending exec
	./main mask exec
	make clean
	

