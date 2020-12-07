/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc52751
 * Daniel Batista fc52773
**/

#include "tree_skel.h"
#include "serialization.h"
#include "tree-private.h"

static struct tree_t *tree;

int tree_skel_init(){
    if(tree == NULL){
        tree = tree_create();
    }
    return tree == NULL? -1 : 0; 
}

void tree_skel_destroy(){
    tree_destroy(tree);
}

int invoke(struct message_t *msg){
    if(tree == NULL){
        perror("Erro na criação da tree, invoke()");
        return -1;
    }

    int ret;
    int op = msg -> msg -> opcode;
    int c_type = msg -> msg -> c_type;

    /*************************************PUT*****************************************/

    if(op == MESSAGE_T__OPCODE__OP_PUT && c_type == MESSAGE_T__C_TYPE__CT_ENTRY){
        printf("PUT\n");
        //Cria a data com os dados que estão na mensagem
        struct data_t *data = data_create2(msg -> msg -> data_size, msg -> msg -> data);
        msg -> msg -> c_type = MESSAGE_T__C_TYPE__CT_NONE;
        msg -> msg -> data_size = 0;
        if(tree_put(tree, msg -> msg -> key, data) == 0){ //Se o put for bem sucedido
            msg -> msg -> opcode = MESSAGE_T__OPCODE__OP_PUT+1;
            free(msg -> msg -> key);
            msg -> msg -> data = NULL;
            ret = 0;
        } else { //Caso o put seja mal sucedido
            msg -> msg -> opcode = MESSAGE_T__OPCODE__OP_ERROR;
            ret = -1;
        }
        msg -> msg -> key = NULL;
        data_destroy(data);
        return ret;

    /*************************************SIZE*****************************************/
    } else if(op == MESSAGE_T__OPCODE__OP_SIZE && c_type == MESSAGE_T__C_TYPE__CT_NONE){
        printf("SIZE\n");
        int size = tree_size(tree); //Vai buscar o tamanho da tree
        msg -> msg -> opcode = MESSAGE_T__OPCODE__OP_SIZE+1;
        msg -> msg -> c_type = MESSAGE_T__C_TYPE__CT_RESULT;
        msg -> msg -> tree_size = size;

    /*************************************DEL*****************************************/ 
    } else if(op == MESSAGE_T__OPCODE__OP_DEL && c_type == MESSAGE_T__C_TYPE__CT_KEY){
        printf("DELETE\n");
        char *key = msg -> msg -> key;
        ret = tree_del(tree, key); //Da delete à key
        free(msg -> msg -> key);
        msg -> msg -> key = NULL;
        if(ret == 0){
            msg -> msg -> opcode = MESSAGE_T__OPCODE__OP_DEL+1;
        } else {
            msg -> msg-> opcode = MESSAGE_T__OPCODE__OP_ERROR;
        }
        msg -> msg -> c_type = MESSAGE_T__C_TYPE__CT_NONE;
        return ret;

    /*************************************GET*****************************************/     
    } else if(op == MESSAGE_T__OPCODE__OP_GET && c_type == MESSAGE_T__C_TYPE__CT_KEY){
        printf("GET\n");
        char *key = msg -> msg -> key;
        struct data_t *data = tree_get(tree, key); //Vai buscar a data associada a uma key
        free(key);
        
        msg -> msg -> key = NULL;
        if(data == NULL){ //Caso a key não esteja na arvore
            msg -> msg -> opcode = MESSAGE_T__OPCODE__OP_ERROR;
            msg -> msg -> c_type = MESSAGE_T__C_TYPE__CT_NONE;
            return -1;
        } else { //Caso a key esteja na arvore
            msg->msg->opcode = MESSAGE_T__OPCODE__OP_GET+1;
            msg -> msg -> c_type = MESSAGE_T__C_TYPE__CT_VALUE;
            msg -> msg -> data_size = data -> datasize;
            msg -> msg -> data = strdup(data -> data);
            data_destroy(data);
            return 0;
        }
    
    /*************************************HEIGHT*****************************************/ 
    } else if(op == MESSAGE_T__OPCODE__OP_HEIGHT && c_type == MESSAGE_T__C_TYPE__CT_NONE){
        printf("HEIGHT\n");
        int height = tree_height(tree); //Vai buscar a altura de uma arvore
        msg -> msg -> opcode = MESSAGE_T__OPCODE__OP_HEIGHT+1;
        msg -> msg -> c_type = MESSAGE_T__C_TYPE__CT_RESULT;
        msg -> msg -> tree_height = height;

    /*************************************GETKEYS*****************************************/     
    } else if(op == MESSAGE_T__OPCODE__OP_GETKEYS && c_type == MESSAGE_T__C_TYPE__CT_NONE){
        printf("GETKEYS\n");
        char **keys = tree_get_keys(tree); //Vai buscar as varias keys que estão dentro da arvore
        int size = tree_size(tree);
        msg -> msg -> opcode = MESSAGE_T__OPCODE__OP_GETKEYS + 1;
        msg -> msg -> c_type = MESSAGE_T__C_TYPE__CT_KEYS;
        msg -> msg -> tree_size = size;

        int lengthTotal = 0;
        int i = 0;
        if(size > 0){
            while(keys[i] != NULL){
                lengthTotal += strlen(keys[i]);
                i++;
            }

            i = 0;
            int begin = 0;
            char *new = malloc(lengthTotal + size);

            if(new == NULL){
                perror("Erro na alocação de memoria, getkeys tree_skel.c");
                return -1;
            }

            while(keys[i] != NULL){ //Mete na variavel new todas as keys que estão na arvore separadas por dois pontos
                memcpy(new+begin, keys[i], strlen(keys[i]));
                begin += strlen(keys[i]);
                memcpy(new+begin, ":", sizeof(char));
                begin++;
                i++;
            }
            //As strings têm de acabar com \0
            memcpy(new+begin-1, "\0", sizeof(char));
            msg -> msg -> key = new;
        }
        tree_free_keys(keys); //Liberta a memoria que foi alocada anteriormente
    }
    return 0;
}