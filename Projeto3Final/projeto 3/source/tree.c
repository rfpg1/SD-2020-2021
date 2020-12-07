/**
Ricardo Gonçalves fc52765
Rafael Abrantes fc52751
Daniel Batista fc52773
Grupo 29
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "entry.h"
#include "tree.h"
#include "tree-private.h"

struct tree_t *tree_create(){
    struct tree_t *root =  malloc(sizeof(struct tree_t));
    root -> value = NULL;
    root -> nodeLeft = NULL;
    root -> nodeRight = NULL;
    return root;
}

void tree_destroy(struct tree_t *tree){
    if(tree != NULL){ 
        if(tree -> value != NULL)
            entry_destroy(tree -> value);

        tree_destroy(tree -> nodeLeft);
        tree_destroy(tree -> nodeRight);

        free(tree);
    }
}

int tree_put(struct tree_t *tree, char *key, struct data_t *value){
    struct entry_t *entry = entry_create(strdup(key), data_dup(value));
    int result = 0;
    if(entry == NULL){
        return -1;  
    }
    int size = tree_size(tree);
    struct data_t *data = tree_get(tree, key);
    if(data != NULL){
        result = 1;
    }
    data_destroy(data);
    tree_insert(tree, entry);
    int temp = tree_size(tree);
    if(result == 1){
        if(temp == size){
            return 0;
        } else {
            return -1;
        }
    } else {
        if(temp == size){
            return -1;
        } else {
            return 0;
        }
    }
}

void tree_insert (struct tree_t *tree, struct entry_t *entry){
    if(tree != NULL){
        if(tree -> value != NULL){
            if(entry_compare(tree -> value, entry) == 0){
                entry_destroy(tree -> value);
                tree -> value = entry;
            } else if(entry_compare(tree -> value, entry) > 0){
                if(tree -> nodeLeft != NULL){
                    tree_insert(tree -> nodeLeft, entry);
                } else {
                    tree -> nodeLeft = tree_create();
                    tree -> nodeLeft -> value = entry;
                }
            } else {
                if(tree -> nodeRight != NULL){
                    tree_insert(tree -> nodeRight, entry);
                } else {
                    tree -> nodeRight = tree_create();
                    tree -> nodeRight -> value = entry;
                }
            }
        } else {
            tree -> value = entry;
        }
    }
}

int tree_size(struct tree_t *tree){
    
    if(tree == NULL){
        return 0;
    } else if(tree -> value == NULL){
        return 0;
    } else {
        return (tree_size(tree -> nodeLeft) + tree_size(tree -> nodeRight) + 1);
    }
}

struct data_t *tree_get(struct tree_t *tree, char *key){
    if(tree != NULL){
        if(tree -> value != NULL && tree -> value -> key != NULL){
            if(strcmp(tree -> value -> key, key) == 0){
                return data_dup(tree -> value -> value);
            }
            else if(strcmp(tree -> value -> key, key) > 0){
                if(tree -> nodeLeft != NULL){
                    return tree_get(tree -> nodeLeft, key);
                } else {
                    return NULL;
                }
            }
            else if(strcmp(tree -> value -> key, key) < 0){
                if(tree -> nodeRight != NULL){
                    return tree_get(tree -> nodeRight, key);
                } else {
                    return NULL;
                }
            }
        }
    }
    return NULL;
}

int tree_del(struct tree_t *tree, char *key){
    if(tree == NULL){
        return -1;
    }
    int result = 0;
    struct data_t *data = tree_get(tree, key);
    if(data != NULL){
        result = 1;
    }

    data_destroy(data);
    if(result == 1){    
        int sizeInit = tree_size(tree);
        tree = tree_delete(tree, key);
        if(tree == NULL) {
            tree = tree_create();
        }
        int sizeFinal = tree_size(tree);
        if(sizeInit != sizeFinal){
            return 0;
        } else {
            return -1;
        }
    } else {
        printf("A key não existe!\n");
        return -1;
    }
    
}

struct tree_t *tree_delete(struct tree_t *tree, char *key){
    if(tree == NULL){
        return tree;
    } 
    if (tree != NULL && tree -> value == NULL){
        return tree;
    }
    if(strcmp(tree -> value -> key, key) > 0){
        tree -> nodeLeft = tree_delete(tree -> nodeLeft, key);
    } else if(strcmp(tree -> value -> key, key) < 0){
        tree -> nodeRight = tree_delete(tree -> nodeRight, key);
    } else if(strcmp(tree -> value -> key, key) == 0){   
        if(tree -> nodeLeft == NULL){
            
            struct tree_t *temp = tree -> nodeRight; //Null caso não haja filhos

            entry_destroy(tree -> value);
            tree -> value = NULL;
            if(temp != NULL){
                tree -> value = temp -> value;
                tree -> nodeRight = temp -> nodeRight;
                tree -> nodeLeft = temp -> nodeLeft;
                free(temp);
                return tree;
            } else {
                return NULL;
            }
            

        } else if(tree -> nodeRight == NULL){
            struct tree_t *temp = tree -> nodeLeft;
            entry_destroy(tree -> value);
            tree -> value = NULL;
            if(temp != NULL){
                tree -> value = temp -> value;
                tree -> nodeRight = temp -> nodeRight;
                tree -> nodeLeft = temp -> nodeLeft;
                free(temp);
                return tree;  
            } else {
                return NULL;
            }
            
        }
        //2 Filhos
        struct tree_t *temp = minValueNode(tree -> nodeRight);
        entry_destroy(tree -> value);
        tree -> value = entry_dup(temp -> value);
        tree -> nodeRight = tree_delete(tree -> nodeRight, temp -> value -> key);
    }
    return tree;
}

struct tree_t *minValueNode (struct tree_t *tree){
    struct tree_t *current = tree;
    while(current -> nodeLeft != NULL){
        current = current -> nodeLeft;
    }
    return current;
}

int tree_height(struct tree_t *tree){
    if(tree == NULL || tree -> value == NULL){
        return 0;
    } else {
        int lDepth = tree_height(tree -> nodeLeft);
        int rDepth = tree_height(tree -> nodeRight);

        if(lDepth > rDepth){
            return (lDepth +1);
        } else {
            return (rDepth +1);
        }
    }
}

char **tree_get_keys(struct tree_t *tree){
    char **str = malloc((tree_size(tree) + 1) * sizeof(char*));
    str[tree_size(tree)] = NULL; //Meter a ultima posição a NULL
    if(str != NULL)
        return tree_get_chaves(tree, str, 0);
    else
        return NULL;
}

char **tree_get_chaves(struct tree_t *tree, char **str, int i){
    if(tree != NULL){
        if(tree -> value != NULL){
            str[i] = strdup(tree -> value -> key);
        }
        if(tree -> nodeLeft != NULL){ 
            tree_get_chaves(tree -> nodeLeft, str, i + 1);
        }
        if(tree -> nodeRight != NULL){
            tree_get_chaves(tree -> nodeRight, str, i + tree_size(tree -> nodeLeft) + 1);
        }
    }
    return str;
}

void tree_free_keys(char **keys){
    int i;
    for(i = 0; keys[i] != NULL; i++){
        free(keys[i]);
    }
    free(keys);
}