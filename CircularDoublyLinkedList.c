/* Copyright 2021 <Tulpan Andrei> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "/home/student/CircularDoublyLinkedList.h"
#include "/home/student/utils.h"

// Functia initializeaza lista si aloca memoria necesara
doubly_linked_list_t*
dll_create(unsigned int data_size)
{
	doubly_linked_list_t *list;
	list = malloc(sizeof(doubly_linked_list_t));
	DIE(!list, "Brain Damage!\n");
	list->head = NULL;
	list->data_size = data_size;
	list->size = 0;
	return list;
}

// Functie pentru returnarea a n-ului nod
dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, int n)
{
	dll_node_t *node;
	if (n < 0) {
		fprintf(stderr, "Error! (n < 0)\n");
		return NULL;
	}
	if (list == NULL || list->head == NULL) {
		fprintf(stderr, "Error2!\n");
		return NULL;
	}
	if (n == 0) {
		return list->head;
	}
	if (n >= list->size) {
		n = n % list->size;
	}
	node = list->head;
	/* Varianta cu parcurgerea unei jumatati */
	if (n <= list->size / 2) {
	    for (int i = 0; i < n; i++) {
	        node = node->next;
	    }
	} else {
	    for (int i = 0; i < list->size - n; i++) {
	        node = node->prev;
	    }
	}
	/* Varianta cu parcurgerea intregii liste */
	// for (int i = 0; i < n; i++) {
	// 	node = node->next;
	// }
	return node;
}

// Functie pentru adaugarea unui nod in lista
void
dll_add_nth_node(doubly_linked_list_t* list, int n, const void* data)
{
	dll_node_t *new_node;
	dll_node_t *node1;
	if (n < 0) {
		fprintf(stderr, "Error! (n < 0)\n");
		return;
	}
	new_node = malloc(sizeof(dll_node_t));
	DIE(!new_node, "Brain Damage!\n");
	new_node->data = malloc(list->data_size);
	DIE(!new_node->data, "Brain Damage!\n");
	memcpy(new_node->data, data, list->data_size);
	if (list->size == 0) {
		list->head = new_node;
		new_node->next = list->head;
		new_node->prev = list->head;
		list->size = 1;
		return;
	}
	if (n == 0) {
		new_node->next = list->head;
		new_node->prev = list->head->prev;
		list->head->prev->next = new_node;
		list->head->prev = new_node;
		list->head = new_node;
		list->size++;
		return;
	}
	if (n >= list->size) {
		new_node->next = list->head;
		new_node->prev = list->head->prev;
		list->head->prev->next = new_node;
		list->head->prev = new_node;
	} else {
		node1 = dll_get_nth_node(list, n - 1);
		new_node->next = node1->next;
		new_node->prev = node1;
		node1->next->prev = new_node;
		node1->next = new_node;
	}
	list->size++;
}

// Functie pentru eliminarea unui nod din lista
dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, int n)
{
	dll_node_t *node;
	if (n < 0 || list->size == 0) {
		fprintf(stderr, "Error1!\n");
		return NULL;
	}
	if (list->size == 1) {
		list->size--;
		dll_node_t *ret = list->head;
		list->head = NULL;
		return ret;
	}
	if (n == 0) {
		node = list->head;
		node->next->prev = list->head->prev;
		node->prev->next = node->next;
		list->head = node->next;
		list->size--;
		return node;
	}
	if (n >= list->size)
		n = list->size - 1;
	node = dll_get_nth_node(list, n);
	node->next->prev = node->prev;
	node->prev->next = node->next;
	list->size--;
	return node;
}

// Functia returneaza numarul de elemente al listei
unsigned int
dll_get_size(doubly_linked_list_t* list)
{
	/* int size = 0;
	if (list->head != NULL) {
		size = 1;
		dll_node_t *node = list->head;
		while (node->next != list->head) {
			size++;
			node = node->next;
		}
	}
	return size; */
	return list->size;
}

// Functia elibereaza memoria intregii liste
void
dll_free(doubly_linked_list_t** pp_list)
{
	int i;
	for (i = 0; i < (*pp_list)->size; i++) {
		void *tmp = (*pp_list)->head->next;
		free((*pp_list)->head->data);
		free((*pp_list)->head);
		(*pp_list)->head = tmp;
	}
	free((*pp_list));
}

// Functia printeaza elementele listei (doar pentru int-uri)
void
dll_print_int_list(doubly_linked_list_t* list)
{
	int i;
	dll_node_t *node;
	node = list->head;
	for (i = 0; i < list->size; i++) {
		printf("%d ", (*(int *)node->data));
		node = node->next;
	}

	printf("\n");
}

// Functia printeaza elementele listei (doar pentru string-uri)
void
dll_print_string_list(doubly_linked_list_t* list)
{
	int i;
	dll_node_t *node;
	node = list->head;
	for (i = 0; i < list->size; i++) {
		printf("%s ", (char *)node->data);
		node = node->next;
	}

	printf("\n");
}
