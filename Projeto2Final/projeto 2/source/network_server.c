/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc52751
 * Daniel Batista fc52773
**/

#include "network_server.h"
#include <arpa/inet.h>
#include <signal.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>

static int IDSocket;

int network_server_init(short port){

    struct sockaddr_in server;
    //Cria o socket TCP
    IDSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(IDSocket < 0){
        perror("Erro na ligação do servidor network_server_init");
        return -1;
    }

    int enable = 1;
    if(setsockopt(IDSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) <0){
        perror("Erro na ligação do servidor network_server_init");
        return -1;
    }
    // Preenche estrutura server com endereço(s) para associar (bind) à socket 
    server.sin_family = AF_INET;
    server.sin_port = port; // Porta TCP
    server.sin_addr.s_addr = htonl(INADDR_ANY); // Todos os endereços na máquina

    //Faz Bind
    if(bind(IDSocket, (struct sockaddr *) &server, sizeof(server)) <0){
        perror("Erro a fazer bind");
        close(IDSocket);
        return-1;
    }

    // Esta chamada diz ao SO que esta é uma socket para receber pedidos
    if(listen(IDSocket, 0) < 0){
        perror("Erro ao executar listen");
        close(IDSocket);
        return -1;
    }

    return IDSocket;
}

int network_main_loop(int listening_socket){
    struct sockaddr_in client;

    socklen_t size_client = 0;

    int connsockfd;

    // accept bloqueia à espera de pedidos de conexão.
    // Quando retorna já foi feito o "three-way handshake" e connsockfd é uma socket pronta a comunicar com o cliente.
    while((connsockfd = accept(listening_socket, (struct sockaddr *) &client, &size_client)) != -1){
        while(1){
            //Vai buscar a mesagem que foi recebida pela rede
            struct message_t *msg = network_receive(connsockfd);
            if(msg == NULL){
                perror("Erro ao receber mesagem do cliente network_main_loop");
                break;
            }
            //Invoca a função que foi passada pela rede
            if(invoke(msg) < 0){
                perror("Erro ao invocar funcao na tree");
            }
            //Envia a resposta pela rede para o cliente
            if(network_send(connsockfd, msg) < 0){
                perror("Erro ao enviar mensagem ao cliente.");
                break;
            }
        }

        // Fecha socket referente a esta conexão
        close(connsockfd);
    }
    network_server_close();
    return 0;
}

struct message_t *network_receive(int client_socket){
    void *data;
    int size;

    struct message_t *msg = malloc(sizeof(struct message_t));

    if(msg == NULL){
        perror("Erro ao alocar memoria network_receive");
        return NULL;
    }

    // Lê tamanho serializado da mensagem serializada no socket
    if(read_all(client_socket, &size, sizeof(int)) < 0){
        perror("Erro ao ler o tamanho da mensagem no socket, read_all, netowork_receive");
        free(msg);
        return NULL;
    }

    size = ntohl(size);
    data = malloc(size);

    //Lê a mensagem enviada pelo cliente
    if(read_all(client_socket, data, size) < 0){
        perror("Erro ao ler o que está no socket, read_all, netowork_receive");
        return NULL;
    }

    msg -> msg = message_t__unpack(NULL, size, data);
    free(data);
    return msg;
}

int network_send(int client_socket, struct message_t *msg){
    int size = message_t__get_packed_size(msg -> msg);
    unsigned char *data = malloc(size);

    if(data == NULL){
        perror("Erro na alocacao de memoria, network_send");
        return -1;
    }

    message_t__pack(msg -> msg, data);

    int len = size;
    size = htonl(size);

    //escreve tamanho serializado no socket
    if(write_all(client_socket, &size, sizeof(int)) < 0){
        return -1;
    }

    //escreve mensagem no socket
    if(write_all(client_socket, data, len) < 0){
        return -1;
    }
    free(data);
    message_t__free_unpacked(msg -> msg, NULL);
    free(msg);
    return 0;
}

int network_server_close(){
    //Fecha a socket
    return close(IDSocket);
}