/* Copyright : Alexandru Olteanu (alexandruolteanu77@gmail.com) */
#include <stdlib.h>
#include <string.h>

#include "server.h"

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

server_memory* init_server_memory() {
    server_memory *aux = (server_memory *)(malloc(sizeof(server_memory)));
	for(int i = 0; i < 50; ++i){
		aux->hashmap[i] = (hash_memory *)malloc(sizeof(hash_memory));
		aux->hashmap[i]->value = (char *)malloc(1024 * sizeof(char));
		aux->hashmap[i]->key = (char *)malloc(1024 * sizeof(char));
		strcpy(aux->hashmap[i]->key, "a");
		strcpy(aux->hashmap[i]->value, "a");  // initierea memoriei unui
		aux->hashmap[i]->used = 0;  // server!
	}
	return aux;
}

void server_store(server_memory* server, char* key, char* value) {
	unsigned int u = hash_function_key(key);
	int x = u % 50;  // fac stocarea unui produs intr-un hashmap
	short ok = 0;
	for(int i = x; i < 50 && !ok; ++i){
		if(server->hashmap[i]->used == 0){
			ok = 1;
			server->hashmap[i]->used = 1;
			int j1 = 0;
			while(key[j1] != '\0'){
				server->hashmap[i]->key[j1] = key[j1];
				++j1;
			}
			server->hashmap[i]->key[j1] = '\0';
			j1 = 0;
			while(value[j1] != '\0'){
				server->hashmap[i]->value[j1] = value[j1];
				++j1;
			}
			server->hashmap[i]->value[j1] = '\0';
		}
	}  // daca nu pot stoca produsul pana la ultimul element din hashmap
	for(int i = 0; i < x && !ok; ++i){  // parcurg circular pornind de la 0
		if(server->hashmap[i]->used == 0){
			ok = 1;
			server->hashmap[i]->used = 1;
			int j1 = 0;
			while(key[j1] != '\0'){
				server->hashmap[i]->key[j1] = key[j1];
				++j1;
			}
			server->hashmap[i]->key[j1] = '\0';
			j1 = 0;
			while(value[j1] != '\0'){
				server->hashmap[i]->value[j1] = value[j1];
				++j1;
			}
			server->hashmap[i]->value[j1] = '\0';
		}
	}
}

void server_remove(server_memory* server, char* key) {
	for(int i = 0; i < 50; ++i){
		if(!strcmp(server->hashmap[i]->key, key)){
			server->hashmap[i]->used = 0;
		}  // este de ajuns sa spun ca respectivul bucket nu este utilizat,
	}  // nu trebuie sa eliberez neaparat memoria
}

char* server_retrieve(server_memory* server, char* key) {
	unsigned int u = hash_function_key(key);
	int x = u % 50;  // caut produsul dupa cheie in server, desigur cu
	short ok = 0;  // conditia ca used sa fie = 1, altfel pot aparea probleme
		for(int j = x; j < 50 && !ok; ++j){
			if(server->hashmap[j]->used){
				if(!strcmp(server->hashmap[j]->key, key)){
					ok = 1;
					return server->hashmap[j]->value;
				}
			}
		}  // la fel ca inainte, parcurg circular hashmap-ul
		for(int j = 0; j < x && !ok; ++j){
			if(server->hashmap[j]->used){
				if(!strcmp(server->hashmap[j]->key, key)){
					ok = 1;
					return server->hashmap[j]->value;
				}
			}
		}
	return NULL;
}

void free_server_memory(server_memory* server) {
	for(int i = 0; i < 50; ++i){  // dau free fiecarui element din hashmap si
		free(server->hashmap[i]->key);  // apoi serverului in sine
		free(server->hashmap[i]->value);
		free(server->hashmap[i]);
	}
	free(server);
}
