#NAME: Bingxinzhu
#EMAIL:bingxinzhu@ucla.edu
#ID: 704845969

CC=gcc
files=lab1b-server.c lab1b-client.c Makefile README my.key

default: lab1b-client lab1b-server

lab1b-server: 
	$(CC) -lmcrypt -o lab1b-server -g lab1b-server.c -Wall -Wextra

lab1b-client: 
	$(CC) -lmcrypt -o lab1b-client -g lab1b-client.c -Wall -Wextra

clean: 
	rm -rf lab1b-server lab1b-client *.gz *.o

dist: 
	tar -zcvf lab1b-704845969.tar.gz $(files)
