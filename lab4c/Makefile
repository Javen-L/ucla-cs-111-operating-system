#NAME: Bingxin Zhu
#EMAIL: bingxinzhu@ucla.edu
#ID: 704845969

FILES = lab4c_tcp.c lab4c_tls.c Makefile README 

default: tcp tls

tcp:
	gcc -g -lmraa -lm -lssl -Wall -Wextra lab4c_tcp.c -o lab4c_tcp

tls:
	gcc -g -lmraa -lm -lssl -lcrypto -Wall -Wextra lab4c_tls.c -o lab4c_tls

clean:
	rm -f lab4c_tcp lab4c_tls *.tar.gz

dist:
	tar -czf lab4c-704845969.tar.gz $(FILES)	