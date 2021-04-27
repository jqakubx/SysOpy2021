CC = gcc -Wall -std=c99

clean:
	rm -rf sender catcher

all:
	make clean
	$(CC) catcher.c -o catcher
	$(CC) sender.c -o sender
