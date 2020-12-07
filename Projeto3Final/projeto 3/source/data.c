/**
Ricardo Gonçalves fc52765
Rafael Abrantes fc52751
Daniel Batista fc52773
Grupo 29
*/
#include "data.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct data_t *data_create(int size){
    if(size <= 0){
        return NULL;
    }

    struct data_t *dados = malloc(sizeof(struct data_t));

    if(dados == NULL){
        perror("Erro de alocacao de memoria");
        return NULL;
    }
    dados -> data = malloc(size);

    if(dados -> data == NULL){
        perror("Erro de alocacao de memoria");
        return NULL;
    }

    dados -> datasize = size;

    return dados;
}

struct data_t *data_create2(int size, void *data){
    if(size <= 0 || data == NULL){
        return NULL;
    }

    struct data_t *dados = malloc(sizeof(struct data_t));

    dados -> data = data;
    dados -> datasize = size;

    return dados;
}

void data_destroy(struct data_t *data){
    if(data != NULL){
        if(data -> data != NULL){
            free(data -> data);
        }
        free(data);
    }
}

struct data_t *data_dup(struct data_t *data){
    if(data == NULL || data-> datasize <= 0 || data-> data == NULL){
        return NULL;
    }
    struct data_t* result = data_create(data->datasize); //criacao da data_t nova
    memcpy(result->data,data->data,data->datasize); //copia dos dados para a data_t nova
    return result;
}

void data_replace(struct data_t *data, int new_size, void *new_data){
    if(data != NULL){
        if(data -> data != NULL){
            free(data -> data);
        }
        data -> data = new_data;
        data -> datasize = new_size;
    }
}