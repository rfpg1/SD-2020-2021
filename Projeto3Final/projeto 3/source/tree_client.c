/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc52751
 * Daniel Batista fc52773
**/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "client_stub.h"

static struct rtree_t *rtree;

char str[50]; 

void closeClient(){
  rtree_disconnect(rtree);
  exit(0);
}


int main(int argc, char** argv){

    signal(SIGINT,closeClient); //Se fechar do nada, interrupção forçada ele faz a função fechar
    signal(SIGPIPE, SIG_IGN); //Se receber o sinal SIGPIPE, ele ignora (SIG_IGN)

    if(argc != 2){ //Tem de ter exatamente 2 argumentos, ./nomeDaClasse IPServer:Porta
        perror("Chame a função da seguinte maneira: ./tree:client <ip do servidor>:<porta>\n");
        return -1;
    }

    rtree = rtree_connect(argv[1]); //Ligação ao servidor!

    if(rtree == NULL){
        perror("Erro a fazer a ligação ao server");
        return -1;
    }
    //While true, para executar tantos comandos como pretender!
    while(1){
        printf("Insira o comando que pretende executar\n");
        //Lê 50 caracteres do teclado
        if(fgets(str, 50, stdin) == NULL){
            perror("Erro ao ler do stdin");
            return -1;
        }


        char *metodo = strtok(str, " "); //primeira palavra do stdin

        if(strncmp(metodo, "\n", strlen("\n")) != 0){ //Não dar simplesmente enter, se der enter volta ao while
            if(strncmp(metodo, "quit", strlen("quit")) == 0){
                //Desliga-se do servidor
                rtree_disconnect(rtree);
                break;
            /*************************************PUT*****************************************/ 
            }else if(strncmp(metodo, "put", strlen("put")) == 0){
                //Criar a entry
                char *key = strtok(NULL, " "); //ir buscar a 2 palavra do stdin
                char *value = strtok(NULL, "\n"); //ir buscar a 3 palavra do stdin

                if(key == NULL || value == NULL){
                    perror("Falta de argumentos, o put precisa de 2 argumentos, a key e o value!");
                    perror("Ex: put A 2");
                    continue;
                }
                //Cria a data e a entry
                struct data_t *data = data_create2(strlen(value) + 1, strdup(value));
                struct entry_t *entry = entry_create(strdup(key), data);
                //Chama o método para meter a entry na arvore
                int result = rtree_put(rtree, entry);
                if(result < 0){
                    printf("Erro no put!\n");
                } else {
                    printf("A operação put (operação numero %d) foi executada!\n", result);
                }

            /*************************************SIZE*****************************************/ 
            } else if(strncmp(metodo, "size", strlen("size")) == 0){
                //Chama o método para saber o size da tree
                int size = rtree_size(rtree);
                printf("Tree size: %d\n", size);

            /*************************************DEL*****************************************/ 
            } else if(strncmp(metodo, "del", strlen("del")) == 0){
                //Vai buscar a key que é para apagar
                char *key = strtok(NULL, "\n");

                if(key == NULL){
                    perror("Falta de argumentos, o del precisa de 1 argumento, a key!");
                    perror("Ex: del A");
                    continue;
                }

                //Chama o método para apagar a key da tree
                int result = rtree_del(rtree, strdup(key));

                if(result < 0){
                    printf("A key não existe!\n");
                } else {
                    printf("A operacao del (operacao numero %d) foi executada!\n", result);
                }
            
            /*************************************GETKEYS*****************************************/ 
            } else if(strncmp(metodo, "getkeys", strlen("getkeys")) == 0){
                //Chama o método para ir buscar as varias keys da tree
                char ** keys = rtree_get_keys(rtree);
                //Caso isto esteja NULL é porque houve um erro
                if(keys == NULL){
                    printf("A tree esta vazia\n");
                    continue;
                }
                int size = rtree_size(rtree);
                printf("As várias keys são: \n");
                int i = 0;
                if(size > 0){
                    while(keys[i] != NULL){
                        printf("%s ", keys[i]);
                        i++;
                    }
                    //Adiciona um \n para ficar mais bonito
                    printf("\n");
                }
                
                rtree_free_keys(keys);
            
            /*************************************GET*****************************************/ 
            }else if(strncmp(metodo, "get", strlen("get")) == 0){
                //Vai buscar a key que é para saber o data associado
                char *key = strtok(NULL, "\n");

                if(key == NULL){
                    perror("Falta de argumentos, o get precisa de 1 argumento, a key!");
                    perror("Ex: get A");
                    continue;
                }
                //Chama o método para ir buscar a data associada à key
                struct data_t *data = rtree_get(rtree, strdup(key));

                if(data == NULL){
                    printf("A key não existe\n");
                    //free(key);
                } else {
                    printf("O valor associado à key %s é: %s.\n", key, (char *) data -> data);
                    data_destroy(data);
                }

            /*************************************HEIGHT*****************************************/ 
            } else if(strncmp(metodo, "height", strlen("height")) == 0){
                //Chama o método para saber a height da tree
                int height = rtree_height(rtree);
                printf("Tree height: %d\n", height);

            /*************************************VERIFY*****************************************/ 
            } else if(strncmp(metodo, "verify", strlen("verify")) == 0){
                //Operalão a verificar se foi feita
                char *key = strtok(NULL, "\n");

                if(key == NULL){
                    perror("Falta de argumentos, o verify precisa de 1 argumento, a operação!");
                    perror("Ex: verify 1");
                    continue;
                }
                //Chama o método para ir verificar se a operação foi feita
                int verify = rtree_verify(rtree, atoi(key));

                if(verify < 0){
                    printf("A operação %d ainda não foi feita\n", atoi(key));
                } else {
                    printf("A operação %d já foi feita\n", atoi(key));
                }  
                              
            } else {
                printf("Método não existe!\n");
            }
        }

    }
}