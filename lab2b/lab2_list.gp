#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
# lab2b_1.png ... throughput vs number of threads for mutex and spin-lock synchronized list operations.
# lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex-synchronized list operations.
# lab2b_3.png ... successful iterations vs threads for each synchronization method.
# lab2b_4.png ... throughput vs number of threads for mutex synchronized partitioned lists.
# lab2b_5.png ... throughput vs number of threads for spin-lock-synchronized partitioned lists.
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#

# general plot parameters
set terminal png
set datafile separator ","

# 1
set title "List-1: Throughput of Synchronized Lists"
set xlabel "Threads"
set ylabel "Throughput(operations/sec)"
set logscale y 10
set output 'lab2b_1.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'mutex' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'spin' with linespoints lc rgb 'green'


# 2
set title "List-2: Per-operation Time"
set xlabel "Threads"
set ylabel "Time/operation(ns)"
set logscale y 10
set output 'lab2b_2.png'

plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'wait for lock' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'completion time' with linespoints lc rgb 'green'


# 3
set title "List-3: Correct Synchronization of Partitioned Lists"
set xlabel "Threads"
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'

plot \
     "< grep 'list-id-none,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'unprotected' with points lc rgb 'red', \
     "< grep 'list-id-m,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'mutex' with points lc rgb 'green',\
	 "< grep 'list-id-s,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'spin-lock' with points lc rgb 'blue'

# 4
set title "List-4: Throughput of Mutex-Synchronized Partitioned Lists" 
set xlabel "Threads"
set ylabel "Throughput(operations/sec)"
set logscale y 10
set output 'lab2b_4.png'

plot \
     "< grep 'list-none-m,[0-9][2]*,1000,1,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'lists=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9][2]*,1000,4,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'lists=4' with linespoints lc rgb 'green', \
	 "< grep 'list-none-m,[0-9][2]*,1000,8,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'lists=8' with linespoints lc rgb 'blue', \
	 "< grep 'list-none-m,[0-9][2]*,1000,16,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'lists=16' with linespoints lc rgb 'orange'


# 5
set title "List-5: Throughput of Spin-Lock-Synchronized Partitioned Lists" 
set xlabel "Threads"
set ylabel "Throughput(operations/sec)"
set logscale y 10
set output 'lab2b_5.png'

plot \
     "< grep 'list-none-s,[0-9][2]*,1000,1,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'lists=1' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9][2]*,1000,4,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'lists=4' with linespoints lc rgb 'green', \
	 "< grep 'list-none-s,[0-9][2]*,1000,8,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'lists=8' with linespoints lc rgb 'blue', \
	 "< grep 'list-none-s,[0-9][2]*,1000,16,' lab2b_list.csv" using ($2):($5)/($6/1000000000) \
	title 'lists=16' with linespoints lc rgb 'orange'
