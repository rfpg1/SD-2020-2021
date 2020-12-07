/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc52751
 * Daniel Batista fc52773
**/

#define NFDESC 4 // Numero de sockets (uma para listening)
#define TIMEOUT 2000 // em milisegundos

#include "network_server.h"
#include <arpa/inet.h>
#include <signal.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include "inet.h"
#include <errno.h>
#include <poll.h>
#include <fcntl.h>
#include "message-private.h"
#include "network_server-private.h"

int IDSocket;
int fechaServer = 0;


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
    if(listening_socket < 0){
        return -1;
    }

    signal(SIGINT, terminaServidor);

    int ret = 0;
    int nrClientsAtivos = 0; /*numero de sockets em desc_set*/
    int capacidade = 2; /*Capacidade máxima*/
    int index = 0; /*indice onde adicionar a proxima socket em desc_set*/
    struct pollfd *desc_set = malloc(capacidade * sizeof(struct pollfd)); /*array com welcoming socket e sockets clientes*/

    /*-------------------Preparar welcoming_sock--------------------*/
    struct pollfd welcomeSock;
    welcomeSock.fd = listening_socket;
    welcomeSock.events = POLLIN; /*como eh um socket servidor, POLLIN significa nova ligacao recebida*/
    /*--------------------------------------------------------------*/

    //Adicionar welcoming_sock a desc_set
    desc_set[index] = welcomeSock;
    index++;

    printf("A aceitar ligacoes.\n");
    while((ret = poll(desc_set, 1 + nrClientsAtivos, TIMEOUT)) >= 0){

        //Por alguma razao quando se faz signal (Ctrl + c) a funcao poll retorna -1 e a
        //computacao nunca atinge este if, mas deixamos aqui por seguranca
        if(fechaServer){
            free(desc_set);
            return 0;
        }

        //Espera por dados nos sockets abertos
        if(desc_set[0].revents & POLLIN){
            //Estrutura para aceitar cliente
            int connsockfd;
            socklen_t size_client = 0;
            struct sockaddr_in client;

            //Verifica se tem novo pedido de conexão
            if((connsockfd = accept(listening_socket, (struct sockaddr *) &client, &size_client)) == -1){
                continue;
            }

            printf("\tAceitei ligacao do cliente %d.\n", connsockfd);
            //Preparar client_sock
            struct pollfd client_sock;
            client_sock.fd = connsockfd;
            client_sock.events = POLLIN;
            //Adicionar client_sock a desc_set
            /************************************************/
            //Realocar espaço caso o array já esteja cheio
            if(1 + nrClientsAtivos >= capacidade){
                desc_set = realocar(desc_set, capacidade, 1);
                if(desc_set == NULL){
                    return -1;
                }
                capacidade = capacidade * 2;
            }
            /************************************************/
            desc_set[index] = client_sock;
            nrClientsAtivos++;
            index++;

            continue; //tem de se dar continue para que seja feito poll na client_sock e assim client_sock.revents seja inicializado
        }
        for(int i = 1; i < 1 + nrClientsAtivos; i++){ //comeca a 1 porque o desc_set[0] eh o welcoming socket
            //Verifica restantes sockets
            if(desc_set[i].revents & POLLIN){ //desc_set[i] tem dados para ler
                struct message_t *msg;
                
                msg = network_receive(desc_set[i].fd);
                if(msg == NULL || msg -> msg -> opcode == MESSAGE_T__OPCODE__OP_BAD){
                    //Sinal de que a conexão foi fechada pelo cliente
                    close(desc_set[i].fd);
                    printf("\tLigacao com cliente %d terminada.\n", desc_set[i].fd);
                    //Remover client de desc_set
                    //Realocar caso o array esteja demasiado grande
                    if(1 + nrClientsAtivos <= capacidade / 2) {
                        desc_set = realocar(desc_set, capacidade, 0);
                        if(desc_set == NULL){
                            return -1;
                        }
                        capacidade /= 2;
                    }
                    int j;
                    for (j = i; j < 1 + nrClientsAtivos - 1; j++){ //-1 pq vamos retirar um
                        desc_set[j] = desc_set[j+1];
                    }

                    desc_set[j].fd = -1; //poll ignora sockets com fd negativo

                    nrClientsAtivos--;
                    index--;
                    continue;
                } else {
                    printf("\t\tMensagem recebida do cliente %d.\n", desc_set[i].fd);
                    invoke(msg);
                    if(network_send(desc_set[i].fd, msg) == -1){
                        if(msg -> msg != NULL ){
                            if(msg -> msg ->data != NULL){
                                free(msg -> msg -> data);
                            }
                            if(msg-> msg -> key != NULL){
                                free(msg -> msg -> key);
                            }
                        }
                        free(msg);
                        continue;
                    }
                    printf("\t\tResposta enviada ao cliente %d.\n", desc_set[i].fd);
                }
            }
            if(desc_set[i].revents & POLLERR || desc_set[i].revents & POLLHUP){
                close(desc_set[i].fd);
                printf("\tLigacao com cliente %d terminada.\n", desc_set[i].fd);
                //Remover client de desc_set
                //Realocar caso o array esteja demasiado grande
                if(1 + nrClientsAtivos <= capacidade / 2){
                    desc_set = realocar(desc_set, capacidade, 0);
                    if(desc_set == NULL){
                        return -1;
                    }
                    capacidade = capacidade / 2;
                }
                /*--------------------------------------------------------*/
                int j;
                for(j = i; j < 1 + nrClientsAtivos - 1; j++){//-1 pq vamos retirar um
                    desc_set[j] = desc_set[j+1];
                }
                desc_set[j].fd = -1; //poll ignora sockets com fd negativo
                /*------------------------------------------------*/
                nrClientsAtivos--;
                index--;
                continue;
            }
        }
    }

    // Fecha socket referente a esta conexão
    free(desc_set);
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
    int by = read_all(client_socket, &size, sizeof(int));
    if(by < 0){
        perror("Erro ao ler o tamanho da mensagem no socket, read_all, netowork_receive");
        free(msg);
        return NULL;
    }

    size = ntohl(size);
    data = malloc(size);
    if(size < 0 || data == NULL){
        free(msg);
        return NULL;
    }
    by = read_all(client_socket, data, size);
    //Lê a mensagem enviada pelo cliente
    if(by < 0){
        perror("Erro ao ler o que está no socket, read_all, netowork_receive");
        free(data);
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
    printf("Server vai encerrar\n");
    return close(IDSocket);
}

//Signal Handler for SIGINT
void terminaServidor(){
    fechaServer = 1;
}

//Aumenta ou diminiu o tamanho do array de clientes tendo em conta as necessidades
struct pollfd *realocar(struct pollfd *desc_set, int capacidade, int sinal){
    if(sinal == 1){
        desc_set = realloc(desc_set, (capacidade * 2) * sizeof(struct pollfd));
        if(desc_set == NULL){
            return NULL;
        }
    } else {
        desc_set = realloc(desc_set, (capacidade / 2) * sizeof(struct pollfd));
        if(desc_set == NULL){
            return NULL;
        }
    }

    return desc_set;
}