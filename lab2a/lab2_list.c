
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
int thread_number = 1;
int iteration_number = 1;
int list_number = 1;
char test_name[128] = "list";
int opt_yield = 0, opt_syn = 0;
char syn_mode;
char* yield_mode;
pthread_mutex_t mutex;
int lock = 0;
void* (*thread_work)(void* id);
SortedListElement_t* element;
SortedList_t* mylist;





void* func_m(void* id) {
	long long i, j;
	for(i = 0; i < iteration_number; i++){
		pthread_mutex_lock(&mutex);
		SortedList_insert(mylist, &element[(*(int*)id)*iteration_number+i]);
		pthread_mutex_unlock(&mutex);
	}
	for (j = 0; j < iteration_number; j++){
		pthread_mutex_lock(&mutex);
		SortedListElement_t* del_ele = SortedList_lookup(mylist, element[(*(int*)id)*iteration_number+j].key);
		if(SortedList_delete(del_ele)){
			fprintf(stderr, "Corrupted\n");
			exit(2);
		}
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void* func_s(void* id){
	long long i, j;
	for (i = 0; i < iteration_number; i++){
		while(__sync_lock_test_and_set(&lock, 1));
		SortedList_insert(mylist, &element[(*(int*)id)*iteration_number+i]);
		__sync_lock_release(&lock);
	}
	for (j = 0; j < iteration_number; j++){
		while(__sync_lock_test_and_set(&lock, 1));
		SortedListElement_t* del_ele = SortedList_lookup(mylist, element[(*(int*)id)*iteration_number+j].key);
		if(SortedList_delete(del_ele)){
			fprintf(stderr, "Corrupted\n");
			exit(2);
		}
		__sync_lock_release(&lock);
	}
	return NULL;
}
void* func_none(void* id){
	long long i, j;
	for (i = 0; i < iteration_number; i++){
		SortedList_insert(mylist, &element[(*(int*)id)*iteration_number+i]);
	}
	for (j = 0; j < iteration_number; j++){
		SortedListElement_t* del_ele = SortedList_lookup(mylist, element[(*(int*)id)*iteration_number+j].key);
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
	char *key = malloc(keyLength*sizeof(char));  
	int i;
	for (i = 0; i < keyLength ; ++i) {
		key[i] = alphanum[rand() % (sizeof(alphanum)-1)];
	}
	key[keyLength] = '\0';  
	return key;
}





int main(int argc, char* argv[]) {
	int input_char = 0;
	int option_index = 0; 
	struct timespec start_time;
	struct timespec end_time;
	long long nano_second = 0; 
	static struct option long_options[] = {
		{"threads", 	 required_argument, NULL, 't'},
		{"iterations",	 required_argument, NULL, 'i'},
		{"yield",	 required_argument, NULL, 'y'},
		{"sync",	 required_argument, NULL, 's'},
		{0, 0, 0, 0}
	};

	while(1) {
		input_char = getopt_long(argc,argv, "t:i:y:s:", long_options, &option_index);
		if(input_char == -1) break;
		else {
			switch(input_char) {
				case 't' : thread_number = atoi(optarg);
				break;
				case 'i' : iteration_number = atoi(optarg);
				break;
				case 'y' : 
					opt_yield = 1;
					yield_mode = optarg;
					break; 
				case 's' : 
					opt_syn = 1;
					syn_mode = *optarg;
					break;
				default:
					printf("unrecognized argument, please use lab2a_list [options]\n\toptions:\n\t\t--threads\n\t\t--iteration\n\t\t--yield\n\t\t--sync\n\n");
					exit(1);
			}
		}
	}

	// yield option
	if(opt_yield) {
		strcat(test_name, "-");
		int i = 0;
		while(yield_mode[i] != '\0') {
			if(yield_mode[i] == 'i') {
				strcat(test_name, "i");
				opt_yield |= INSERT_YIELD;
			} else if(yield_mode[i] == 'd') {
				strcat(test_name, "d");
				opt_yield |= DELETE_YIELD;
			} else if(yield_mode[i] == 'l'){
				strcat(test_name,"l");
				opt_yield |= LOOKUP_YIELD;
			} else {
				fprintf(stderr, "invalid yield option\n" );
				exit(1);
			}
			i++;
		}
	} else {
		strcat(test_name, "-none");
	}

	// get thread_work function
	if(opt_syn) {
		switch(syn_mode){
			case 'm':
				thread_work = &func_m;
				strcat(test_name, "-m");
				break;
			case 's':
				thread_work = &func_s;
				strcat(test_name, "-s");
				break;
			default:
				fprintf(stderr, "invalid sync option\n" );
				exit(1);
		}
	} else {
		thread_work = &func_none;
		strcat(test_name, "-none");
	}

	// create list
	element= malloc(sizeof(SortedListElement_t) * iteration_number * thread_number);
	int index = 0;
	while(index != thread_number * iteration_number) {
		element[index].key = createRandomKey();
		index ++;
	}
	// create list head
	mylist = malloc(sizeof(SortedList_t));
	mylist->key = NULL;
	mylist->prev = mylist;
	mylist->next = mylist;

	pthread_t* tid = malloc(thread_number * sizeof(pthread_t));
	// get start time
	clock_gettime(CLOCK_MONOTONIC, &start_time);

	// create threads
	int t;
	int *id = malloc(sizeof(int)*thread_number);
	for(t = 0; t < thread_number; t++) {
		id[t] = t;
		if(pthread_create(&tid[t], NULL, thread_work, id + t)){
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}
	}

	//join threads
	for(t = 0; t < thread_number; t++){
		if(pthread_join(tid[t], NULL) < 0) {
			fprintf(stderr, "%s\n", strerror(errno));
			exit(1);
		}
	}

	// get end time
	clock_gettime(CLOCK_MONOTONIC, &end_time);

	// total time
	nano_second = (end_time.tv_sec - start_time.tv_sec)*1000000000 + (end_time.tv_nsec - start_time.tv_nsec);

	// length of the list(should be 0)
	int listlength = SortedList_length(mylist);
	if(listlength) {
		fprintf(stderr, "listlength is not 0\n" );
		exit(2);
	}
	free(element);
	free(tid);
	free(id);
	free(mylist);

	// print cvs

	printf("%s,%d,%d,%d,%d,%lli,%lli\n", test_name, thread_number, iteration_number, list_number,thread_number*iteration_number*3, nano_second, nano_second/(thread_number*iteration_number*3));
	return 0;



}





















