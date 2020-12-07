/**
Ricardo Gonçalves fc52765
Rafael Abrantes fc52751
Daniel Batista fc52773
Grupo 29
*/
#include "entry.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct entry_t *entry_create(char *key, struct data_t *data){
    if(data == NULL || key == NULL || strlen(key) == 0){
        perror("chave ou dados são null");
        return NULL;
    }
    struct entry_t *entrada = malloc(sizeof(struct entry_t));
    if(entrada == NULL){
        return NULL;
    }
    entrada -> key = key;
    entrada -> value = data;

    return entrada;
}
//Pedir testes ao stor
void entry_initialize(struct entry_t *entry){
    if(entry != NULL){
        entry -> key = NULL;
        entry -> value = NULL;
    }
}

void entry_destroy(struct entry_t *entry){
    if(entry != NULL){
        if(entry -> key != NULL){
            free(entry -> key);
        }
        if(entry -> value != NULL){
            data_destroy(entry -> value);
        }
        free(entry);
    }
}

struct entry_t *entry_dup(struct entry_t *entry){
    if(entry != NULL && entry -> key != NULL && entry ->  value != NULL){
        return entry_create(strdup(entry -> key), data_dup(entry -> value));
    }
    return NULL;
}

void entry_replace(struct entry_t *entry, char *new_key, struct data_t *new_value){
    if(entry != NULL){
        if(entry -> key != NULL){
            free(entry -> key);
        }
        if(entry -> value != NULL){
            data_destroy(entry -> value);
        }
        entry -> key = new_key;
        entry -> value = new_value;
    }
}

int entry_compare(struct entry_t *entry1, struct entry_t *entry2){
    if(strcmp(entry1->key, entry2->key) == 0){
        return 0;
    } else if(strcmp(entry1->key, entry2->key) < 0){
        return -1;
    } else {
        return 1;
    }
}