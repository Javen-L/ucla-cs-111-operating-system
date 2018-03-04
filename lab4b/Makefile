#NAME: Bingxin Zhu
#EMAIL: bingxinzhu@ucla.edu
#ID: 704845969

FILES = lab4b.c Makefile README P4B_check.sh

default:
	gcc -g -lmraa -lm -Wall -Wextra lab4b.c -o lab4b

check:
	chmod +x P4B_check.sh
	./P4B_check.sh 704845969

clean:
	rm -f lab4b *.tar.gz

dist:
	tar -czf lab4b-704845969.tar.gz $(FILES)	