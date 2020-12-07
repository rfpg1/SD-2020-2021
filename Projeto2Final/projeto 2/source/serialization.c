/**
Ricardo Gonçalves fc52765
Rafael Abrantes fc52751
Daniel Batista fc52773
Grupo 29
*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "serialization.h"
#include "tree.h"
#include "tree-private.h"

int data_to_buffer(struct data_t *data, char **data_buf){

    if(data == NULL || data_buf == NULL){
        return -1;
    }

    data_buf[0] = malloc(sizeof(int)+data->datasize);
    int offset = 0;
    memcpy(data_buf[0],&(data->datasize),sizeof(int));
    offset += sizeof(int);
    memcpy(data_buf[0]+offset,data->data,data->datasize);
    offset += data->datasize;
    return offset;
}

struct data_t *buffer_to_data(char *data_buf, int data_buf_size){
    if(data_buf == NULL || data_buf_size < 0){
        return NULL;
    }
    int size = 0;
    memcpy(&(size), data_buf, sizeof(int));
    struct data_t *result = data_create(size);
    memcpy(result -> data, data_buf + sizeof(int), size);
    return result;
}

int entry_to_buffer(struct entry_t *entry, char **entry_buf){
    if(entry == NULL || entry_buf == NULL){
        return -1;
    }

    entry_buf[0] = malloc(strlen(entry -> key) + 1 + entry -> value -> datasize + (sizeof(int) *2));
    char **data_buf = malloc(sizeof(char *));
    int offset = 0;
    int tamanhoKey = strlen(entry->key)+1;
    memcpy(entry_buf[0], &tamanhoKey, sizeof(int));
    offset += sizeof(int);
    memcpy(entry_buf[0] + offset, entry -> key, tamanhoKey);
    offset += tamanhoKey;
    int datasize = data_to_buffer(entry -> value, data_buf);
    memcpy(entry_buf[0] + offset, data_buf[0], datasize);
    offset += datasize;
    
    free(data_buf[0]);
    free(data_buf);

    return offset;
}

struct entry_t *buffer_to_entry(char *entry_buf, int entry_buf_size){
    if(entry_buf == NULL || entry_buf_size < 0){
        return NULL;
    }
    int tamanhokey;
    memcpy(&tamanhokey, entry_buf, sizeof(int));
    char *key = malloc(tamanhokey);
    memcpy(key, entry_buf + sizeof(int), tamanhokey);
    struct data_t *data = buffer_to_data(entry_buf + sizeof(int) + tamanhokey, entry_buf_size - sizeof(int) - tamanhokey);
    struct entry_t *entry = entry_create(key, data);
    return entry;
}

int tree_to_buffer_aux(struct tree_t *tree, char **tree_buf, int offset){
    
    char **entry_buf = malloc(sizeof(char *));
    int entrysize = entry_to_buffer(tree -> value, entry_buf);
    int entry_size = (strlen(tree -> value -> key) + 1 + tree -> value -> value -> datasize + (sizeof(int) * 2));

    memcpy(tree_buf[0] + offset, entry_buf[0], entrysize);

    free(entry_buf[0]);
    free(entry_buf);
    offset += entrysize;
    
    int result = offset;
    
    if(tree -> nodeLeft != NULL){
        result += tree_to_buffer_aux(tree -> nodeLeft, tree_buf, offset);
    }
    
    if(tree -> nodeRight != NULL){
        result += tree_to_buffer_aux(tree -> nodeRight, tree_buf, offset + (entry_size * tree_size(tree -> nodeLeft))); 
    }
    printf("\n%d - %d\n", result, tree_size(tree -> nodeLeft));

   return result;
}


int tree_to_buffer(struct tree_t *tree, char **tree_buf){
    if(tree_buf == NULL || tree == NULL){
        return -1;
    }
    int entry_size = sizeof(int) * 2 + sizeof(char *) + sizeof(struct entry *) + sizeof(struct data*);
    int treesize = tree_size(tree);
    tree_buf[0] = malloc(treesize * entry_size);
    return tree_to_buffer_aux(tree, tree_buf, 0);
}

void buffer_to_tree_aux(char *tree_buf, int tree_buf_size, struct tree_t *tree, int offset){
    if(offset > tree_buf_size){
        return;
    }
    int entrysize = (sizeof(int) * 2 + sizeof(char *) + sizeof(struct entry *) + sizeof(struct data*));
    struct entry_t *entry = buffer_to_entry(tree_buf + offset, entrysize);
    tree_put(tree, entry -> key, entry -> value);
    entry_destroy(entry);
    buffer_to_tree_aux(tree_buf, tree_buf_size, tree, offset + entrysize);
}

struct tree_t *buffer_to_tree(char *tree_buf, int tree_buf_size){
    if(tree_buf == NULL|| tree_buf_size < 0)
        return NULL;
    struct tree_t *tree = tree_create();
    buffer_to_tree_aux(tree_buf, tree_buf_size, tree, 0);
    return tree;
}