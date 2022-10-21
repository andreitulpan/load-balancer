/* Copyright 2021 <Tulpan Andrei> */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "/home/student/server.h"

// Initiez memoria pentru replica
server_t* init_server(server_memory *real_server, unsigned int tag) {
	server_t *server = malloc(sizeof(server_t));
	DIE(!server, "malloc failed!\n");
	server->hash_server = real_server;		// referinta pentru serverul real
	server->tag = tag;		// id-ul replicii
	server->hash = hash_function_servers(&tag);		// hash-ul replicii
	return server;
}

// Initiez memoria pentru server
server_memory* init_server_memory() {
	server_memory *servers = malloc(sizeof(server_memory));
	DIE(!servers, "malloc failed!\n");
	// Creez hashtable-ul corespunzator serverului
	servers->server_ht =
	ht_create(997, hash_function_key, compare_function_strings);
	return servers;
}

// Functia pentru stocarea produselor pe server
void server_store(server_memory* server, char* key, char* value) {
	// Apelez functia de adaugare in hashtable
	ht_put(server->server_ht, key, strlen(key) + 1, value, strlen(value) + 1);
}

// Functia pentru stergerea produselor din hashtable
void server_remove(server_memory* server, char* key) {
	// Apelez functia de sterge din hashtable
	ht_remove_entry(server->server_ht, key);
}

// Functia de gasirea de returnare a valorii corespunzatoare cheii
char* server_retrieve(server_memory* server, char* key) {
	// Apelez functia de gasire in hashtable
	return ht_get(server->server_ht, key);
}

// Functia pentru eliberarea memoriei serverului
void free_server_memory(server_memory* server) {
	// Eliberez hashtable-ul
	ht_free(server->server_ht);
	// Eliberez serverul
	free(server);
}
