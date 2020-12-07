/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc52751
 * Daniel Batista fc52773
**/

#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <signal.h>
#include "network_server.h"
#include "tree_skel.h"

void closeClient(){
    tree_skel_destroy();
    network_server_close();
    exit(0);
}

int main (int argc, char **argv) {

    signal(SIGINT,closeClient); //Se fechar do nada, interrupção forçada ele faz a função fechar
    signal(SIGPIPE, SIG_IGN); //Se receber o sinal SIGPIPE, ele ignora (SIG_IGN)

    if(argc != 2){ //Tem de ter exatamente 2 argumentos, ./nomeDaClasse Porta
        printf("Uso: ./treeServer <porto_servidor>\n");
        printf("Exemplo de uso: ./treeServer 12345\n");
        return -1;
    }

    short porta = htons(atoi(argv[1])); //Porta para a ligação TCP

    int socket_escuta = network_server_init(porta); //Ligação TCP

    if(socket_escuta < 0){ //Caso haja erro na ligação TCP
        perror("Erro no socket");
        return -1;
    }

    if(tree_skel_init() < 0){ //Inicializa a tree
        perror("Erro ao inicializar a tree");
        network_server_close();
        return -1;
    }
    
    if(network_main_loop(socket_escuta) < 0){ //Fica à espera até existir uma ligação e depois são invocados vários metodos
        perror("Erro enquanto estava no while(1)");
        tree_skel_destroy();
        network_server_close();
        return -1;
    }

    tree_skel_destroy(); //Destruir a tree
    network_server_close(); //Fechar a ligação
    return 0;
}
