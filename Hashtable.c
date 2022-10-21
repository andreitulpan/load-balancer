/* Copyright 2021 <Tulpan Andrei> */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "/home/student/Hashtable.h"
#include "/home/student/utils.h"

#define MAX_BUCKET_SIZE 64

// Functia compara cheile (doar pentru int-uri)
int
compare_function_ints(void *a, void *b)
{
	int int_a = *((int *)a);
	int int_b = *((int *)b);

	if (int_a == int_b) {
		return 0;
	} else if (int_a < int_b) {
		return -1;
	} else {
		return 1;
	}
}

// Functia compara cheile (doar pentru string-uri)
int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

// Functia initializeaza structura de hashtable si memoria necesara
hashtable_t *
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	unsigned int i = 0;
	hashtable_t *hashtable;

	hashtable = malloc(sizeof(hashtable_t));
	DIE(!hashtable, "Hastable allocation failed!\n");

	hashtable->buckets = malloc(hmax * sizeof(linked_list_t *));
	DIE(!hashtable->buckets, "Buckets allocation failed!\n");

	for (i = 0; i < hmax; i++) {
		hashtable->buckets[i] = ll_create(sizeof(struct info));
	}

	hashtable->size = 0;
	hashtable->hmax = hmax;
	hashtable->hash_function = hash_function;
	hashtable->compare_function = compare_function;

	return hashtable;
}

// Functia pune o pereche de cheie-valoare in hashtable
void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	unsigned int hash, i;
	int exist = 0;
	hash = ht->hash_function(key);
	hash %= ht->hmax;
	ll_node_t *node;
	node = ((linked_list_t *)ht->buckets[hash])->head;
	for (i = 0; i < ht->buckets[hash]->size; i++) {
		void *key2 = ((struct info *)node->data)->key;
		int comparation = ht->compare_function(key, key2);
		if (!comparation) {
			struct info *infos = node->data;
			infos->value = realloc(infos->value, value_size);
			memcpy(infos->value, value, value_size);
			exist = 1;
			break;
		}
		node = node->next;
	}
	if (!exist) {
		struct info informations;
		informations.key = malloc(key_size);
		memcpy(informations.key, key, key_size);
		informations.value = malloc(value_size);
		memcpy(informations.value, value, value_size);
		ll_add_nth_node(ht->buckets[hash], ht->buckets[hash]->size, &informations);
		ht->size++;
	}
}

// Functia extrage datele stocate pentru cheia primita
void *
ht_get(hashtable_t *ht, void *key)
{
	unsigned int hash, i;
	hash = ht->hash_function(key);
	hash %= ht->hmax;
	ll_node_t *node;
	node = ((linked_list_t *)ht->buckets[hash])->head;
	for (i = 0; i < ht->buckets[hash]->size; i++) {
		void *key2 = ((struct info *)node->data)->key;
		int comparation = ht->compare_function(key, key2);
		if (!comparation) {
			return ((struct info *)node->data)->value;
		}
		node = node->next;
	}
	return NULL;
}

// Functia returneaza 1 daca exista cheia si 0 in caz contrar
int
ht_has_key(hashtable_t *ht, void *key)
{
	void *value = ht_get(ht, key);
	if (value != NULL) {
		return 1;
	}
	return 0;
}

// Functia sterge o pereche cheie-valoare din hashtable
void
ht_remove_entry(hashtable_t *ht, void *key)
{
	unsigned int hash, i;
	hash = ht->hash_function(key);
	hash %= ht->hmax;
	ll_node_t *node;
	node = ((linked_list_t *)ht->buckets[hash])->head;
	for (i = 0; i < ht->buckets[hash]->size; i++) {
		void *key2 = ((struct info *)node->data)->key;
		int comparation = ht->compare_function(key, key2);
		if (!comparation) {
			ll_node_t *free_node;
			free_node = ll_remove_nth_node(((linked_list_t *)ht->buckets[hash]), i);
			free(((struct info *)free_node->data)->key);
			free(((struct info *)free_node->data)->value);
			free(free_node->data);
			free(free_node);
			ht->size--;
			break;
		}
		node = node->next;
	}
}

// Functia elibereaza memoria alocata hashtable-ului
void
ht_free(hashtable_t *ht)
{
	for (unsigned int i = 0; i < ht->hmax; i++) {
		ll_node_t *node;
		int size = ht->buckets[i]->size;
		for (int j = 0; j < size; j++) {
			node = ll_remove_nth_node(((linked_list_t *)ht->buckets[i]), 0);
			free(((struct info *)node->data)->key);
			free(((struct info *)node->data)->value);
			free(node->data);
			free(node);
		}
		free(ht->buckets[i]);
	}
	free(ht->buckets);
	free(ht);
}

// Functia returneaza numarul de perechi stocate in hashtable
unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

// Functia returneaza numarul de bucket-uri al hashtable-ului
unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
