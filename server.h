/* Copyright 2021 <Tulpan Andrei> */
#ifndef SERVER_H_
#define SERVER_H_

#include "/home/student/LinkedList.h"
#include "/home/student/Hashtable.h"
#include "/home/student/load_balancer.h"
#include "/home/student/utils.h"

// Structura corespunzatoare replicii
typedef struct server_memory server_memory;

struct server_memory {
	hashtable_t *server_ht;
};

// Structura corespunzatoare serverului
typedef struct server_t server_t;

struct server_t {
	server_memory *hash_server;
	unsigned int tag;
	unsigned int hash;
};

server_memory* init_server_memory();

server_t* init_server(server_memory *real_server, unsigned int tag);

void free_server_memory(server_memory* server);

/**
 * server_store() - Stores a key-value pair to the server.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 * @arg3: Value represented as a string.
 */
void server_store(server_memory* server, char* key, char* value);

/**
 * server_remove() - Removes a key-pair value from the server.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 */
void server_remove(server_memory* server, char* key);

/**
 * server_remove() - Gets the value associated with the key.
 * @arg1: Server which performs the task.
 * @arg2: Key represented as a string.
 *
 * Return: String value associated with the key
 *         or NULL (in case the key does not exist).
 */
char* server_retrieve(server_memory* server, char* key);

#endif  // SERVER_H_
