
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sched.h>

int nthread = 1;
int niteration = 1;
long long counter =0;
int opt_yield = 0;
int opt_syn = 0;
char syn_mode;
pthread_mutex_t add_mutex;
int lock = 0;
struct thread_data{
	long long* ptr;
	long long val1;
	long long val2;
};

struct thread_data data;
struct thread_data *dataptr = &data;

void (*add_func_ptr)(long long*, long long);

void add(long long *pointer, long long value){
	long long sum = *pointer + value;
	if(opt_yield)
		sched_yield();
	*pointer = sum;
}

void add_m(long long *pointer, long long value){
	pthread_mutex_lock(&add_mutex);
	add(pointer, value);
	pthread_mutex_unlock(&add_mutex);
}

void add_s(long long *pointer, long long value){
	while(__sync_lock_test_and_set(&lock, 1));
	add(pointer, value);
	__sync_lock_release(&lock);
}

void add_c(long long *pointer, long long value){
	long long old, new;
	do{
		old = *pointer;
		new = old + value;
		if(opt_yield)
			sched_yield();
	}while(__sync_val_compare_and_swap(pointer, old, new)!= old);
}

void* thread_work(void * data){
	struct thread_data *my_data;
	my_data = (struct thread_data *) data;
	int i, j;
	for(i = 0; i < niteration; i++){
		add_func_ptr(my_data->ptr, my_data->val1);
	}
	for(j = 0; j < niteration; j++){
		add_func_ptr(my_data->ptr, my_data->val2);
	}
	return NULL;
}

int main(int argc, char *argv[]){
	int c = 0;
	int option_index = 0;
	struct timespec start_time;
	struct timespec end_time;
	long long ns;
	data.ptr = &counter;
	data.val1 = 1;
	data.val2 = -1;
	char *test_name;

	static struct option long_options[] = {
		{"threads", required_argument, NULL, 't'},
		{"iterations", required_argument, NULL, 'i'},
		{"yield", no_argument, NULL, 'y'},
		{"sync", required_argument, NULL, 's'},
		{0, 0, 0, 0}
	};

	while(1){
		c = getopt_long(argc, argv, "t:i:ys:", long_options, &option_index);
		if(c == -1)
			break;
		switch(c){
			case 't':
			nthread = atoi(optarg);
			break;
			case 'i':
			niteration = atoi(optarg);
			break;
			case 'y':
			opt_yield = 1;
			break;
			case 's':
			opt_syn = 1;
			syn_mode = optarg[0];
			break;
			default:
			printf("unrecognized argument, please use lab2a_add [options]\n\toptions:\n\t\t--threads\n\t\t--iteration\n\t\t--yield\n\t\t--sync\n\n");
			exit(1);
		}
	}

	if(opt_syn){
		switch(syn_mode){
		case 'm':
		add_func_ptr = &add_m;
		if(opt_yield){
			test_name = "add-yield-m";
		}
		else{
			test_name = "add-m";
		}	
		break;
		case 's':
		add_func_ptr = &add_s;
		if(opt_yield){
            test_name = "add-yield-s";
		}
		else{
            
			test_name = "add-s";
		}	
		break;
		case 'c':
		add_func_ptr = &add_c;
		if(opt_yield){
            
			test_name = "add-yield-c";
		}
		else{
            
			test_name = "add-c";
		}	
		break;
		default:
		printf("invaid synchronization mode, please use m, s, or c\n");
		exit(1);
		}
	}
	else{
		add_func_ptr = &add;
		if(opt_yield){
            
			test_name = "add-yield-none";
		}
		else{
			test_name = "add-none";
		}	
	}

	// get start time
	clock_gettime(CLOCK_MONOTONIC, &start_time);

	// create threads
	pthread_t thread[nthread];
	int t, rc;
	for(t = 0; t < nthread; t++){
		rc = pthread_create(&thread[t], NULL, thread_work, (void *)dataptr);
		if(rc){
			printf("Error, return code from pthread_create() is %d\n", rc);
			exit(1);
		}
	}

	// join threads
	for(t = 0; t < nthread; t++){
		rc = pthread_join(thread[t], NULL);
		if(rc){
			printf("Error, return code from pthread_join() is %d\n", rc);
			exit(1);
		}
	}

	// get end time
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	ns = (end_time.tv_sec - start_time.tv_sec) * 1000000000 + (end_time.tv_nsec - start_time.tv_nsec);

	// print result in csv format
	printf("%s,%d,%d,%d,%lli,%lli,%lli\n", test_name, nthread, niteration, nthread*niteration*2, ns, ns/(nthread*niteration*2), counter);

	return 0;

}

