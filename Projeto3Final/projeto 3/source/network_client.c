/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc52751
 * Daniel Batista fc52773
**/

#include "message-private.h"
#include "sdmessage.pb-c.h"
#include "client_stub-private.h"

int network_connect(struct rtree_t *rtree){
    //Cria o socket TCP
    int sockfd;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Erro ao criar o socket");
        return -1;
    }
    //Mete o id do socket na arvore remota
    rtree -> sockfd = sockfd;
    //Estabelece a ligação com o servidor!
    if(connect(sockfd, (struct sockaddr *)rtree -> sock_in, sizeof(struct sockaddr)) <0 ){
        close(sockfd);
        return -1;
    }

    return 0;
}

struct message_t *network_send_receive(struct rtree_t * rtree, struct message_t *msg){

    int sockfd = rtree -> sockfd; //Socket
    void *buffer = NULL; //Pedido serializado
    struct message_t *resposta; 
    unsigned tamanho; //Tamanho do pedido
    int numeroDeBytes;

    //Aloca memoria para a messagem
    resposta = malloc(sizeof(struct message_t));
    if(resposta < 0){
        perror("Erro ao alocar memoria");
        return NULL;
    }
    //Vai buscar o tamanho da mensagem
    tamanho = message_t__get_packed_size(msg -> msg);
    buffer = malloc(tamanho);
    if(buffer == NULL){
        perror("Erro ao alocar memoria");
        return NULL;
    }
    
    tamanho = htonl(tamanho);
    message_t__pack(msg -> msg, buffer);

    //Envia o tamanho da mensagem
    if(write_all(sockfd,&tamanho,sizeof(int)) < 0){
        perror("Erro ao escrever no socket write_all");
        free(buffer);
        return NULL;
    }

    tamanho = ntohl(tamanho);
    //Envia a mensagem
    numeroDeBytes = write_all(sockfd, buffer, tamanho);
    if(numeroDeBytes < 0){
        perror("Erro ao escrever no socket write_all");
        free(buffer);
        return NULL;
    }

    free(buffer);

    //Lê o tamanho da mensagem serializada
    
    if(read_all(sockfd,&tamanho,sizeof(int)) < 0){
        perror("Erro ao escrever no socket read_all.");
        return NULL;
    }

    tamanho = ntohl(tamanho);
    buffer = malloc(tamanho);

    //Lê a mensagem serializada
    if(read_all(sockfd,buffer,tamanho) < 0){
        perror("Erro ao escrever no socket read_all.");
        free(buffer);
        return NULL;
    }

    resposta -> msg = message_t__unpack(NULL,tamanho,buffer);
    free(buffer);
    return resposta;
}

int network_close(struct rtree_t * rtree){
    //Fecha o socket
    return close(rtree -> sockfd);
}