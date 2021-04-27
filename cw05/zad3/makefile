CC = gcc -std=c99 -Wall

all:
	make clean
	$(CC) -o producer producer.c
	$(CC) -o consumer consumer.c
	$(CC) -o test test.c

clean:
	rm -f producer consumer test fifo

test_all:
	make clean
	make all
	./test