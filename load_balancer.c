/* Copyright 2021 <Tulpan Andrei> */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "/home/student/load_balancer.h"

// Structura pentru load balancer
struct load_balancer {
    doubly_linked_list_t *servers;
};

// Functia hash pentru servere
unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

// Functia de hash pentru produse
unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

// Functia de initiere pentru load balancer
load_balancer* init_load_balancer() {
    // Aloc load_balancer-ul
    load_balancer *lb = malloc(sizeof(load_balancer));
    DIE(!lb, "malloc failed!\n");
    // Aloc lista inlantuita pentru replicile serverelor
    lb->servers = dll_create(sizeof(server_t));
    return lb;
}

// Functia pentru stocarea produselor pe server
void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
    dll_node_t *node = main->servers->head;
    unsigned int hash = hash_function_key(key);
    // Ajung la serverul potrivit pentru hash-ul produsului
    for (int i = 0; i < main->servers->size; i++) {
        if (((server_t *)node->data)->hash > hash)
            break;
        node = node->next;
    }
    // Returnez server id-ul serverului
    *server_id = ((server_t *)node->data)->tag % 100000;
    // Apelez functia de stocare pentru serverul respectiv
    server_memory *real_server = ((server_t *)node->data)->hash_server;
    server_store(real_server, key, value);
}

// Functia pentru a gasi valoarea corespunzatoare unei chei
char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	dll_node_t *node = main->servers->head;
    unsigned int hash = hash_function_key(key);
    // Ajung la serverul corespunzator hash-ului
    for (int i = 0; i < main->servers->size; i++) {
        if (((server_t *)node->data)->hash > hash) {
            break;
        }
        node = node->next;
    }
    // Returnez id-ul serverului pe care se afla produsul
    *server_id = ((server_t *)node->data)->tag % 100000;
    // Apelez functia de gasire a produsului pe server
    server_memory *real_server = ((server_t *)node->data)->hash_server;
	return server_retrieve(real_server, key);
}

// Functia de adaugare a unui server
void loader_add_server(load_balancer* main, int server_id) {
    // Initiez memoria pentru server
    server_memory *real_server = init_server_memory();

    // Initiez cele 3 replici ale serverului real
    server_t *server[3];
    server[0] = init_server(real_server, server_id);
    server[1] = init_server(real_server, 100000 + server_id);
    server[2] = init_server(real_server, 200000 + server_id);

    dll_node_t *node;
    int counter;
    // Adaug fiecare replica a serverului real
    for (int i = 0; i < 3; i++) {
        // Ajung la pozitia corespunzatoare replicii pe care o voi adauga
        node = main->servers->head;
        counter = 0;
        while (counter < main->servers->size &&
               server[i]->hash > ((server_t *)node->data)->hash) {
            counter++;
            node = node->next;
        }

        // Adaug replica pe hashring
        if (main->servers->size < 3) {  // nu remapez elementele
            dll_add_nth_node(main->servers, counter, server[i]);
        } else {  // remapez elementele
            dll_add_nth_node(main->servers, counter, server[i]);

            // Replica anterioara serverului inainte de adaugare
            server_t *prev_copy = ((server_t *)node->prev->prev->data);
            // Replica serverului curent
            server_t *curr_copy = ((server_t *)node->data);

            // Serverul de pe care voi remapa produsele
            server_memory *curr_server = curr_copy->hash_server;

            /* Parcurg hashtable-ul serverului si salvez
               un pointer pentru fiecare produs intr-un vector */
            unsigned int ht_size = curr_server->server_ht->size;
            // Aloc vectorul de pointeri
            struct info **entries = malloc(sizeof(struct info *) * ht_size);
            DIE(!entries, "malloc failed!\n");
            int position = 0;
            unsigned int bucket_size;
            ll_node_t *product;

            // Parcurgerea hashtable-ului
            for (unsigned int j = 0; j < curr_server->server_ht->hmax; j++) {
                product = (curr_server->server_ht->buckets[j]->head);
                bucket_size = curr_server->server_ht->buckets[j]->size;
                for (unsigned int k = 0; k < bucket_size; k++) {
                    entries[position] = (struct info *)product->data;
                    position++;
                    product = product->next;
                }
            }

            // Copiez produsele de la adresele salvate in vector
            for (int j = 0; j < position; j++) {
                char *key1 = (char *)entries[j]->key;
                char *value1 = (char *)entries[j]->value;

                char *key = malloc(strlen(key1) + 1);
                DIE(!key, "malloc failed!\n");
                memcpy(key, key1, strlen(key1) + 1);
                char *value = malloc(strlen(value1) + 1);
                DIE(!value, "malloc failed!\n");
                memcpy(value, value1, strlen(value1) + 1);

                unsigned int key_hash = hash_function_key(key);
				// Verific daca produsul se afla intre hash-urile corespunzatoare
                if (key_hash >= prev_copy->hash
                    && key_hash < server[i]->hash) {
                    server_remove(curr_server, key);
                    server_store(server[i]->hash_server, key, value);
                } else if (node->prev == main->servers->head
                           && key_hash >= prev_copy->hash) {
                    server_remove(curr_server, key);
                    server_store(server[i]->hash_server, key, value);
                } else if (node->prev == main->servers->head
                           && key_hash < server[i]->hash) {
                    server_remove(curr_server, key);
                    server_store(server[i]->hash_server, key, value);
                }

                free(key);
                free(value);
            }
            // Eliberez vectorul alocat anterior
            free(entries);
        }
        // Eliberez memoria alocata replicii
        free(server[i]);
    }
}

// Functia de stergere a unui server
void loader_remove_server(load_balancer* main, int server_id) {
	// Calculez hash-ul pentru toate replicile serverului
    unsigned int hash[3];
    int counter;
    int id = server_id;
    hash[0] = hash_function_servers(&id);
    id += 100000;
    hash[1] = hash_function_servers(&id);
    id += 100000;
    hash[2] = hash_function_servers(&id);

    server_memory *real_server;

    // Sterg fiecare replica a serverului
    for (int i = 0; i < 3; i++) {
        dll_node_t *node;
        node = main->servers->head;
        counter = 0;

        // Ajung la pozitia corespunzatoare replicii serverului
        while (hash[i] != ((server_t *)node->data)->hash
               && counter < main->servers->size) {
            counter++;
            node = node->next;
        }

        // Replica curenta
        server_t *curr_copy = ((server_t *)node->data);

        // Salvez un pointer catre serverul real
        real_server = curr_copy->hash_server;

        // Sterg replica de pe hashring
        dll_node_t *remove = dll_remove_nth_node(main->servers, counter);

        // Eliberez memoria replicii
        free(remove->data);
        free(remove);
    }

    /* Parcurg hashtable-ul corespunzator serverului si salvez
       un pointer catre fiecare element intr-un vector */
    unsigned int ht_size = real_server->server_ht->size;
    // Aloc vectorul de pointeri
    struct info **entries = malloc(sizeof(struct info *) * ht_size);
    DIE(!entries, "malloc failed!\n");
    int position = 0;
    unsigned bucket_size;

    // Salvez pointeri catre produse
    for (unsigned int j = 0; j < real_server->server_ht->hmax; j++) {
        ll_node_t *product;
        product = (real_server->server_ht->buckets[j]->head);
        bucket_size = real_server->server_ht->buckets[j]->size;
        for (unsigned int k = 0; k < bucket_size; k++) {
            entries[position] = (struct info *)product->data;
            position++;
            product = product->next;
        }
    }

    // Remapez produsele pe serverele corespunzatoare
    for (int j = 0; j < position; j++) {
        char *key = (char *)entries[j]->key;
        char *value = (char *)entries[j]->value;
        int id = 0;
        loader_store(main, key, value, &id);
    }
    // Eliberez memoria vectorului alocat anterior
    free(entries);
    // Sterg si eliberez memoria serverului real
    free_server_memory(real_server);
}

// Functia de eliberare e memoriei
void free_load_balancer(load_balancer* main) {
    // Parcurg lista dublu inlantuita de servere
    int size = main->servers->size;
    dll_node_t *node = main->servers->head;
    for (int i = 0; i < size; i++) {
        node = main->servers->head;
        // Eliberez memoria alocata pentru serverele reale
        if (((server_t *)node->data)->tag < 100000) {
            free_server_memory(((server_t *)node->data)->hash_server);
        }
        // Eliberez memoria alocata pentru replici
        dll_node_t *remove = dll_remove_nth_node(main->servers, 0);
        // Eliberez datele fiecarei replici
        free(remove->data);
        free(remove);
    }
    // Eliberez lista dublu inalantuita a replicilor
    free(main->servers);
    // Eliberez memoria alocata pentru load balancer
    free(main);
}
