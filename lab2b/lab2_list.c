
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "SortedList.h"
#include <signal.h>
#include <unistd.h>
#include <limits.h>

int nthread = 1;
int niteration = 1;
int nlist = 1;
char test_name[128] = "list";
int opt_yield = 0, opt_syn = 0, opt_list = 0;
char syn_mode;
char* yield_mode;
pthread_mutex_t* mutex;
int* lock ;
void* (*thread_work)(void* id);
SortedListElement_t* element;
SortedList_t* mylist;
long long ns = 0;
long long ns_sum_m = 0; 
long long ns_sum_s = 0;


unsigned int hash_function(const char *key){
	unsigned int seed = 131;
	unsigned int hash = 0;
	size_t i;
	for(i = 0; i < strlen(key); i++){
		hash = (hash * seed) + key[i];
	}
	return (hash % nlist);
}

void* func_m(void* id){
	long long i, j, k;
	struct timespec start_time;
	struct timespec end_time;
	int len_sum = 0;
	
	for (i = 0; i < niteration; i++){
		long long ns;
		unsigned int key = hash_function(element[(*(int*)id)*niteration+i].key);

		clock_gettime(CLOCK_MONOTONIC, &start_time);
		pthread_mutex_lock(&mutex[key]);
		clock_gettime(CLOCK_MONOTONIC, &end_time);

		ns = (end_time.tv_sec - start_time.tv_sec)*1000000000 + (end_time.tv_nsec - start_time.tv_nsec);
		ns_sum_m += ns;

		SortedList_insert(&mylist[key], &element[(*(int*)id)*niteration+i]);
		pthread_mutex_unlock(&mutex[key]);
	}

	//int len_sum = 0;
	for (k = 0; k < nlist; k++){
		long long ns;

		clock_gettime(CLOCK_MONOTONIC, &start_time);
		pthread_mutex_lock(&mutex[k]);
		clock_gettime(CLOCK_MONOTONIC, &end_time);

		ns = (end_time.tv_sec - start_time.tv_sec)*1000000000 + (end_time.tv_nsec - start_time.tv_nsec);
		ns_sum_m += ns;

	}
	for(k = 0; k < nlist; k++){
		len_sum += SortedList_length(&mylist[k]);
	}
	for(k = 0; k < nlist; k++){
		pthread_mutex_unlock(&mutex[k]);
	}
	

	for (j = 0; j < niteration; j++){
		long long ns;
		unsigned int key = hash_function(element[(*(int*)id)*niteration+j].key);

		clock_gettime(CLOCK_MONOTONIC, &start_time);
		pthread_mutex_lock(&mutex[key]);
		clock_gettime(CLOCK_MONOTONIC, &end_time);

		ns = (end_time.tv_sec - start_time.tv_sec)*1000000000 + (end_time.tv_nsec - start_time.tv_nsec);
		ns_sum_m += ns;

		SortedListElement_t* del_ele = SortedList_lookup(&mylist[key], element[(*(int*)id)*niteration+j].key);
		if(SortedList_delete(del_ele)){
			fprintf(stderr, "Corrupted\n");
			exit(2);
		}
		pthread_mutex_unlock(&mutex[key]);
	}
	return NULL;
}
void* func_s(void* id){
	long long i, j, k;
	struct timespec start_time;
	struct timespec end_time;
	int len_sum = 0;

	for (i = 0; i < niteration; i++){
		long long ns;
		unsigned int key = hash_function(element[(*(int*)id)*niteration+i].key);

		clock_gettime(CLOCK_MONOTONIC, &start_time);
		while(__sync_lock_test_and_set(&lock[key], 1));
		clock_gettime(CLOCK_MONOTONIC, &end_time);

		ns = (end_time.tv_sec - start_time.tv_sec)*1000000000 + (end_time.tv_nsec - start_time.tv_nsec);
		ns_sum_s += ns;

		SortedList_insert(&mylist[key], &element[(*(int*)id)*niteration+i]);
		__sync_lock_release(&lock[key]);
		
	}

	for (k = 0; k < nlist; k++){
		long long ns;

		clock_gettime(CLOCK_MONOTONIC, &start_time);
		while(__sync_lock_test_and_set(&lock[k], 1));
		clock_gettime(CLOCK_MONOTONIC, &end_time);

		ns = (end_time.tv_sec - start_time.tv_sec)*1000000000 + (end_time.tv_nsec - start_time.tv_nsec);
		ns_sum_s += ns;

	}
	for(k = 0; k < nlist; k++){
		len_sum += SortedList_length(&mylist[k]);
	}
	for(k = 0; k < nlist; k++){
		__sync_lock_release(&lock[k]);
	}

	for (j = 0; j < niteration; j++){
		long long ns;
		unsigned int key = hash_function(element[(*(int*)id)*niteration+j].key);

		clock_gettime(CLOCK_MONOTONIC, &start_time);
		while(__sync_lock_test_and_set(&lock[key], 1));
		clock_gettime(CLOCK_MONOTONIC, &end_time);

		ns = (end_time.tv_sec - start_time.tv_sec)*1000000000 + (end_time.tv_nsec - start_time.tv_nsec);
		ns_sum_s += ns;

		SortedListElement_t* del_ele = SortedList_lookup(&mylist[key], element[(*(int*)id)*niteration+j].key);
		if(SortedList_delete(del_ele)){
			fprintf(stderr, "Corrupted\n");
			exit(2);
		}
		__sync_lock_release(&lock[key]);
	}
	return NULL;
}
void* func_none(void* id){
	long long i, j, k;
	//struct timespec start_time;
	//struct timespec end_time;
	int len_sum = 0;

	for (i = 0; i < niteration; i++){
		unsigned int key = hash_function(element[(*(int*)id)*niteration+i].key);
		SortedList_insert(&mylist[key], &element[(*(int*)id)*niteration+i]);
	}

	for(k = 0; k < nlist; k++){
		len_sum += SortedList_length(&mylist[k]);
	}

	for (j = 0; j < niteration; j++){
		unsigned key = hash_function(element[(*(int*)id)*niteration+j].key);
		SortedListElement_t* del_ele = SortedList_lookup(&mylist[key], element[(*(int*)id)*niteration+j].key);
		if(SortedList_delete(del_ele)){
			fprintf(stderr, "Corrupted\n");
			exit(2);
		}
	}
	return NULL;
}

char* createRandomKey(){
	char *alphanum = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuv0123456789";
	int keyLength = 10;
	char *key = malloc((1+keyLength) * sizeof(char));  
	//char *key = malloc(keyLength * sizeof(char)); 
	int i;
	for (i = 0; i < keyLength ; ++i) {
		key[i] = alphanum[rand() % (sizeof(alphanum)-1)];
	}
	key[keyLength] = '\0';  
	return key;
}

void signal_handler(int signum){
	if(signum == SIGSEGV){
		fprintf(stderr, "Error:Segmentation fault\n");
		exit(2);
	}
}



int main(int argc, char* argv[]){
	int c = 0;
	int option_index = 0;
	struct timespec start_time;
	struct timespec end_time;
	//long long ns = 0;

	static struct option long_options[] = {
		{"threads", 	 required_argument, NULL, 't'},
		{"iterations",	 required_argument, NULL, 'i'},
		{"yield",	 required_argument, NULL, 'y'},
		{"sync",	 required_argument, NULL, 's'},
		{"lists",	required_argument, NULL, 'l'},
		{0, 0, 0, 0}
	};

	while(1){ 
		c = getopt_long(argc, argv, "t:i:y:s:l:", long_options, &option_index);
		if (c == -1)
			break;
		switch (c){
			case 't':
				nthread = atoi(optarg);
				break;
			case 'i':
				niteration = atoi(optarg);
				break;
			case 'y':
				opt_yield = 1;
				yield_mode = optarg;
				break;
			case 's':
				opt_syn = 1;
				syn_mode = *optarg;
				break;
			case 'l':
				opt_list = 1;
				nlist = atoi(optarg);
				break;
			default:
				printf("unrecognized argument, please use lab2a_list [options]\n\toptions:\n\t\t--threads\n\t\t--iteration\n\t\t--yield\n\t\t--sync\n\n");
				exit(1);

		}
	}

	signal(SIGSEGV, signal_handler);
	// yield option
	if(opt_yield){
		strcat(test_name, "-");
		int i = 0;
		while(yield_mode[i] != '\0'){
			if(yield_mode[i] == 'i'){
				strcat(test_name, "i");
				opt_yield |= INSERT_YIELD;
			}
			else if(yield_mode[i] == 'd'){
				strcat(test_name, "d");
				opt_yield |= DELETE_YIELD;
			}
			else if(yield_mode[i] == 'l'){
				strcat(test_name, "l");
				opt_yield |= LOOKUP_YIELD;
			}
			else{
				fprintf(stderr, "invaid yield option\n");
				exit(1);
			}
			i++;
		}
	}
	else
		strcat(test_name, "-none");

	// get thread_work function
	if(opt_syn){
		switch(syn_mode){
			case 'm':
				mutex = malloc(sizeof(pthread_mutex_t)*nlist);
				int i;
				for(i = 0; i < nlist; i++){
					pthread_mutex_init(&mutex[i], NULL); 
				}
				thread_work = &func_m;
				strcat(test_name, "-m");
				break;
			case 's':
				lock = malloc(sizeof(int)*nlist);
				thread_work = &func_s;
				strcat(test_name, "-s");
				break;
			default:
				fprintf(stderr, "invaid sync option\n");
				exit(1);
		}
	}
	else{
		thread_work = &func_none;
		strcat(test_name, "-none");		
	}

	// create list
	element = malloc(sizeof(SortedListElement_t)*niteration*nthread);
	int index = 0;
	while(index != nthread*niteration){
		element[index].key = createRandomKey();
		index ++;
	}

	// create list head
	mylist = malloc(sizeof(SortedList_t)*nlist);
	int k;
	for(k = 0; k < nlist; k++){
		mylist[k].key = NULL;
		mylist[k].prev = &mylist[k];
		mylist[k].next = &mylist[k];
	}


	pthread_t* tid = malloc(nthread*sizeof(pthread_t));
	// get start time
	clock_gettime(CLOCK_MONOTONIC, &start_time); 


	// create threads
	int t;
	int *id = malloc(sizeof(int)*nthread);
	for (t = 0; t < nthread; t++){
		id[t] = t;
		if (pthread_create(&tid[t], NULL, thread_work, id+t)){
			fprintf(stderr, "%s\n" ,strerror(errno));
			exit(1);			
		}
	}

	// join threads
	for (t = 0; t < nthread; t++){
		if (pthread_join(tid[t], NULL) < 0){
			fprintf(stderr, "%s\n" ,strerror(errno));
			exit(1);			
		}
	}

	// get end time
	clock_gettime(CLOCK_MONOTONIC, &end_time);
	

	// total time
	ns = (end_time.tv_sec - start_time.tv_sec)*1000000000 + (end_time.tv_nsec - start_time.tv_nsec);

	// length of list (should be 0)
	int listlength = 0;
	for(k = 0; k < nlist; k++){
		listlength += SortedList_length(&mylist[k]);
	}
	if (listlength){
		fprintf(stderr, "List length is not 0\n");
		exit(2);
	}

	free(element);
	free(tid);
	free(id);
	free(mylist);

	// print in csv format
	printf("%s,%d,%d,%d,%d,%lli,%lli,", test_name, nthread, niteration, nlist,nthread*niteration*3, ns, ns/(nthread*niteration*3));
	if(opt_syn){
		if(syn_mode == 'm'){
			fprintf(stdout, "%lli\n", ns_sum_m/(niteration*nthread));
		}
		else if(syn_mode == 's'){
			fprintf(stdout, "%lli\n", ns_sum_s/(niteration*nthread));
		}
		
	}
	else
			fprintf(stdout, "0\n");
	return 0;
}

