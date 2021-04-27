CC = gcc -Wall 

clean:
	rm -Rf *.o *.out main mainstop mainreset

test:
	make clean
	$(CC) -o main main.c
	$(CC) -o mainstop mainsa_nocldstop.c
	$(CC) -o mainreset mainresethand.c
	./main
	./mainstop
	./mainreset
	make clean
	

