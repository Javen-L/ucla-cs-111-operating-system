#NAME : bingxinzhu
#Email : bingxinzhu@g.ucla.edu 
#ID : 704845969
Default:
	gcc -o lab0 -Wall -Wextra -g lab0.c

check : copyCheck check2
 
copyCheck:
	echo “It is a warm up project. But it is still difficult.” > inputText.txt
	./lab0 --input=inputText.txt --output=outputTest.txt
	(diff inputText.txt outputTest.txt)||(echo “Copy Failed!”)
	rm -f inputText.txt outputTest.txt
	
check2:
	./lab0 --segfault --catch ; \
	if [ $$? -eq 4 ] ; \
	then \
		echo “Successed. ” ; \
	else \
		echo “Failed. “ ; \
	fi

clean:
	rm -f lab0 *.txt lab0-704845969.tar.gz *.o

dist:
	tar -cf lab0-704845969.tar.gz lab0.c Makefile backtrace.png breakpoint.png README