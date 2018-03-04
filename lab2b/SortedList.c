#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
	SortedListElement_t *cur = list->next;
	if(list == NULL || element == NULL){
		fprintf(stderr, "list or element do not exist\n");
		return;
	}
	
	// find the insert position
	while(cur != list && (strcmp(element->key, cur->key)>0)){
		if(opt_yield & INSERT_YIELD)
			sched_yield();
		cur = cur->next;
	}
	if(opt_yield & INSERT_YIELD)
		sched_yield();
	// insert the element
	element->next = cur;
	element->prev = cur->prev;
	cur->prev->next = element;
	cur->prev = element;
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element){
	if(element == NULL)
		return 1;
	//check corruption
	if(element->prev->next != element){
		return 1;
	}
	if(element->next->prev != element){
		return 1;
	}
	if(opt_yield & DELETE_YIELD)
		sched_yield();
		//delete
	element->prev->next = element->next;
	element->next->prev = element->prev;
	element->next = NULL;
	element->prev = NULL;
	return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
	if(list == NULL || key == NULL)
		return NULL;
	SortedListElement_t *cur = list->next; 
	if(opt_yield & LOOKUP_YIELD)
			sched_yield();
	while(cur != list){
		if(strcmp(key, cur->key) == 0){
			return cur;
		}
		
		cur = cur->next;
	}
	return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *	While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list){
	if(list == NULL){
		return 0;
	}
	int length = 0;
	SortedListElement_t *cur = list->next;

	if(opt_yield & LOOKUP_YIELD)
		sched_yield();

	while(cur != list){
		
		if(cur->prev->next != cur->next->prev)
			return -1;
		length++;
		cur = cur->next;
	}
	return length;

}

