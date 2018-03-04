#NAME: Bingxin Zhu
#EMAIL: bingxinzhu95@gmail.com
#ID: 704845969

FILES = lab2_list.c SortedList.h SortedList.c *.png Makefile README *.csv *.gp profile.out

default:
	gcc -Wall -Wextra -pthread -g -o lab2_list SortedList.c lab2_list.c
	

tests:
	#@rm -f *.csv
	@./lab2_list --threads=1  --iterations=1000 --sync=m >> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2b_list.csv
	@./lab2_list --threads=12  --iterations=1000 --sync=m >> lab2b_list.csv
	@./lab2_list --threads=16  --iterations=1000 --sync=m >> lab2b_list.csv
	@./lab2_list --threads=24  --iterations=1000 --sync=m >> lab2b_list.csv
	@./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2b_list.csv
	@./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2b_list.csv
	@./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2b_list.csv
	@./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2b_list.csv
	@./lab2_list --threads=12  --iterations=1000 --sync=s >> lab2b_list.csv
	@./lab2_list --threads=16  --iterations=1000 --sync=s >> lab2b_list.csv
	@./lab2_list --threads=24  --iterations=1000 --sync=s >> lab2b_list.csv

	@-./lab2_list --threads=1 --iterations=1 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=1 --iterations=2 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=1 --iterations=4 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=1 --iterations=8 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=1 --iterations=12 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=1 --iterations=16 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=4 --iterations=1 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=4 --iterations=2 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=4 --iterations=4 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=4 --iterations=8 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=4 --iterations=12 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=4 --iterations=16 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=8 --iterations=1 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=8 --iterations=2 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=8 --iterations=4 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=8 --iterations=8 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=8 --iterations=12 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=8 --iterations=16 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=1 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=2 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=4 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=8 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=12 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=12 --iterations=16 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=1 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=2 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=4 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=8 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=12 --lists=4 --yield=id >>  lab2b_list.csv
	@-./lab2_list --threads=16 --iterations=16 --lists=4 --yield=id >>  lab2b_list.csv

	@./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=m >>  lab2b_list.csv

	@./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=s >>  lab2b_list.csv

	@./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=1000 --lists=4 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=1000 --lists=8 --sync=m >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=1000 --lists=16 --sync=m >>  lab2b_list.csv

	@./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=1000 --lists=4 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=1000 --lists=8 --sync=s >>  lab2b_list.csv
	@./lab2_list --threads=16 --iterations=1000 --lists=16 --sync=s >>  lab2b_list.csv


graphs:
	#chmod +x lab2_list.gp
	@gnuplot ./lab2_list.gp 

profile:
	-rm -f ./raw.gperftools
	LD_PRELOAD=/usr/lib64/libprofiler.so CPUPROFILE=./raw.gperftools ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperftools > profile.out
	pprof --list=func_s ./lab2_list ./raw.gperftools >> profile.out
	rm -f ./raw.gperf

clean: 
	rm -rf lab2_list *.o *.perf *.tar.gz profile 

dist: 
	make
	make tests
	make graphs
	make profile
	tar -czf lab2b-704845969.tar.gz $(FILES)

