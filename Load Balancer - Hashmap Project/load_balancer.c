/* Copyright : Alexandru Olteanu (alexandruolteanu77@gmail.com) */
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"

// Aici avem functiile pentru hash predefinite in scheletul de cod
unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

load_balancer* init_load_balancer() {
    load_balancer* aux = (load_balancer *)malloc(sizeof(load_balancer));
	aux->nr_servers = 0;  // aloc memorie pentru load balancer
    return aux;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
    unsigned int x = hash_function_key(key);  // cautam primul serever
	int n = main->nr_servers, ok = 0;  // convenabil pentru a stoca produsul
	for(int i = 0; i < n && !ok; ++i){
		if(main->servers_all[i]->value >= x){
			ok = 1;
			server_store(main->servers_all[i], key, value);
			*server_id = main->servers_all[i]->id;
		}
	}
	if(!ok){  // daca nu il putem stoca in mod obisnuit, inseamna ca circular
		server_store(main->servers_all[0], key, value);  // va ajunge in
		*server_id = main->servers_all[0]->id;  // serverul 0
	}
}


char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
    int n = main->nr_servers;
	for(int i = 0; i < n; ++i){
		if(server_retrieve(main->servers_all[i], key) != NULL){
			*server_id = main->servers_all[i]->id;
			return server_retrieve(main->servers_all[i], key);
		}  // parcurg toate serverele si apelez functia"server_retrieve"
	}  // pentru a parcurge fiecare server individual
	return NULL;
}

void loader_add_server(load_balancer* main, int server_id) {
    for (unsigned int i = 0; i < 3; ++i){
		unsigned int eticheta = i * 1e5 + server_id;
		unsigned int x = hash_function_servers(&eticheta);
		main->servers_all[main->nr_servers] = init_server_memory();
		short pos = -1;
		for(int j = 0; j < main->nr_servers && pos < 0; ++j){
			if(x <= main->servers_all[j]->value){
				if(x == main->servers_all[j]->value){
					if(server_id < main->servers_all[j]->id){
						pos = j;
					}  // stabilesc pozitia unde trebuie stocat noul server
				}
				else
					pos = j;
			}
		}
		if(pos == -1){
			pos = main->nr_servers;
		}  // In continuare, fac mutare spre dreapta pentru a face loc
		for(int j = main->nr_servers - 1; j >= pos; --j){  // noului server
			main->servers_all[j + 1]->id = main->servers_all[j]->id;
			main->servers_all[j + 1]->value = main->servers_all[j]->value;
			for(int t = 0; t < 50; ++t){
			   main->servers_all[j + 1]->hashmap[t]->used =
			   main->servers_all[j]->hashmap[t]->used;
			   int j1 = 0;
			   while (main->servers_all[j]->hashmap[t]->key[j1] != '\0'){
			   		main->servers_all[j + 1]->hashmap[t]->key[j1] =
			   		main->servers_all[j]->hashmap[t]->key[j1];
			   		++j1;
			   }
			   main->servers_all[j + 1]->hashmap[t]->key[j1] = '\0';
			   j1 = 0;
			   while (main->servers_all[j]->hashmap[t]->value[j1] != '\0'){
			   		main->servers_all[j + 1]->hashmap[t]->value[j1] =
			   		main->servers_all[j]->hashmap[t]->value[j1];
			   		++j1;
			   }
			   main->servers_all[j + 1]->hashmap[t]->value[j1] = '\0';
			}
		}
		main->servers_all[pos]->id = server_id;
		main->servers_all[pos]->value = x;
		for(int t = 0; t < 50; ++t){
			main->servers_all[pos]->hashmap[t]->used = 0;
		}
			int to = pos + 1;
			if(pos == main->nr_servers){
				to = 0;
			}  // verific pentru fiecare produs de pe server-ul urmator daca
			main->nr_servers++;  // trebuie remapat sau nu catre serverul
			for(int j = 0; j < 50; ++j){  // curent
				if(main->servers_all[to]->hashmap[j]->used){
						server_remove(main->servers_all[to],
							main->servers_all[to]->hashmap[j]->key);
						int x = 0;
						loader_store(main,
							main->servers_all[to]->hashmap[j]->key,
							main->servers_all[to]->hashmap[j]->value, &x);
				}
			}
	}
}


void loader_remove_server(load_balancer* main, int server_id) {
	for(int i = 0; i < 3; ++i){
		int n = main->nr_servers;
		short ok = 0;  // pentru fiecare dintre cele 3 replici, sterg serverul
		for(int j = 0; j < n && !ok; ++j){  // si remapez produsele
			if(main->servers_all[j]->id == server_id){  // aflate pe el
				int to = j + 1;
				if(to >= n){
					to = 0;
				}
				for(int t = 0; t < 50; ++t){
					if(main->servers_all[j]->hashmap[t]->used){
						server_store(main->servers_all[to],
							main->servers_all[j]->hashmap[t]->key,
							main->servers_all[j]->hashmap[t]->value);
						server_remove(main->servers_all[j],
							main->servers_all[j]->hashmap[t]->key);
					}
				}
				ok = 1;  // mut toate serverele necesare spre stanga pentru a
				for(int t = j; t < n - 1; ++t){  // pastra continuitatea
					main->servers_all[t]->id = main->servers_all[t + 1]->id;
					main->servers_all[t]->value =
					main->servers_all[t + 1]->value;
					for(int t1 = 0; t1 < 50; ++t1){
						main->servers_all[t]->hashmap[t1]->used =
						main->servers_all[t + 1]->hashmap[t1]->used;
						int j1 = 0;
			   			while(main->servers_all[t + 1]->
			   				hashmap[t1]->key[j1] != '\0'){
			   				main->servers_all[t]->hashmap[t1]->key[j1] =
			   			main->servers_all[t + 1]->hashmap[t1]->key[j1];
			   				++j1;
			   			}
			   			main->servers_all[t]->hashmap[t1]->key[j1] = '\0';
			   			j1 = 0;
			   			while(main->servers_all[t + 1]->hashmap[t1]->value[j1]
			   				!= '\0'){
			   				main->servers_all[t]->hashmap[t1]->value[j1] =
			   				main->servers_all[t + 1]->hashmap[t1]->value[j1];
			   				++j1;
			   			}
			   			main->servers_all[t]->hashmap[t1]->value[j1] = '\0';
					}
				}
				main->nr_servers--;  // decrementez numarul de servere si
				free_server_memory(main->servers_all[n - 1]);  // eliberez
			}  // memoria pentru nodul eliminat
		}
	}
}

void free_load_balancer(load_balancer* main) {
    for (int i = 0; i < main->nr_servers; ++i){
		free_server_memory(main->servers_all[i]);
	}  // eliberez memoria pentru fiecare server in parte si apoi pentru
	free(main);  // intregul load_balancer
}
