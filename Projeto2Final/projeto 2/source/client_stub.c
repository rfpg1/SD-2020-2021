/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc5251
 * Daniel Batista fc5273 
**/

#include "client_stub-private.h"


struct rtree_t *rtree_connect(const char *address_port){

    struct rtree_t *rtree;
    struct sockaddr_in *server;

    //Aloca memoria para o rtree
    rtree = malloc(sizeof(struct rtree_t));
    if(rtree == NULL){
        perror("Erro na alocação de memória no rtree_connect rtree_t");
        return NULL;
    }

    //Aloca memoria para o server
    server = malloc(sizeof(struct sockaddr_in));
    if(server == NULL){
        perror("Erro na alocação de memória no rtree_connect sockaddr_in");
        return NULL;
    }

    //Copia o IPAdress e a Porta para uma nova variavel
    char *addressEport = strdup(address_port);
    //Pega nessa nova variavel e separa-a por : para ir buscar o ipadress
    char *address = strtok(addressEport, ":");
    //Vai separar outra vez a variavel por \n para ir buscar a porta
    char *port = strtok(NULL, "\n");
    
    // Preenche estrutura server com endereço do servidor para estabelecer conexão
    server -> sin_family = AF_INET; //Endereços!
    server -> sin_port = htons(atoi(port)); //Porta TCP

    if(inet_pton(AF_INET, address, &server -> sin_addr.s_addr) < 0){ //Setup ao endereço IP
        perror("Erro ao fazer inet_pton rtree_connect");
        free(server);
        free(rtree);
        return NULL;
    }

    //Liga o server à arvore remota
    rtree -> sock_in = server;

    free(addressEport);

    //Liga-se ao socket
    if(network_connect(rtree) < 0){
        perror("Erro ao fazer ligação TPC com server");
        free(server);
        free(rtree);
        return NULL;
    }

    return rtree;
}

int rtree_disconnect(struct rtree_t *rtree){
    //Fecha a ligação com o servidor
    if(network_close(rtree) < 0){
        perror("Erro a fechar a ligação TCP");
        return -1;
    }

    free(rtree -> sock_in);
    free(rtree);
    return 0;
}

int rtree_put(struct rtree_t *rtree, struct entry_t *entry){
    struct message_t *pedido = malloc(sizeof(struct message_t));
    MessageT *msg = malloc(sizeof(MessageT));
    //Aloca memoria para as estruturas de dados
    if(pedido == NULL || msg == NULL){
        perror("Erro ao alocar memoria no rtree_put");
        return -1;
    }

    message_t__init(msg);

    msg -> opcode = MESSAGE_T__OPCODE__OP_PUT; //OPCode associado 
    msg -> c_type = MESSAGE_T__C_TYPE__CT_ENTRY; //C_TYPE associado
    msg -> data_size = entry -> value -> datasize;
    msg -> data = strdup(entry -> value -> data);
    msg -> key = strdup(entry -> key);

    pedido -> msg = msg;
    //Envia a mensagem para o servidor e recebe a resposta do mesmo
    struct message_t *resposta = network_send_receive(rtree, pedido);

    if(resposta == NULL){
        perror("Erro ao mandar o pedido pela rede rtree_put\n");
        return -1;
    }

    message_t__free_unpacked(msg, NULL);
    entry_destroy(entry);
    free(pedido);
    //Tendo em conta a operação ser bem ou mal sucedida
    int ret = -1;
    if(resposta -> msg -> opcode == MESSAGE_T__OPCODE__OP_PUT + 1 && resposta -> msg -> c_type == MESSAGE_T__C_TYPE__CT_NONE){
        ret = 0;
    }

    message_t__free_unpacked(resposta->msg, NULL);
    free(resposta);
    return ret;
}

struct data_t *rtree_get(struct rtree_t *rtree, char *key){
    struct message_t *pedido = malloc(sizeof(struct message_t));
    MessageT *msg = malloc(sizeof(MessageT));
    //Aloca memoria para as estruturas de dados
    if(msg == NULL || pedido == NULL){
        perror("Erro ao alocar memoria no rtree_get");
        return NULL;
    }
    message_t__init(msg);

    msg -> opcode = MESSAGE_T__OPCODE__OP_GET; //OPCode associado 
    msg -> c_type = MESSAGE_T__C_TYPE__CT_KEY; //C_TYPE associados

    msg -> key = key;

    pedido -> msg = msg;
    //Envia a mensagem para o servidor e recebe a resposta do mesmo
    struct message_t *resposta = network_send_receive(rtree, pedido);

    if(resposta == NULL) {
        perror("Erro ao mandar o pedido pela rede rtree_get\n");
        return NULL;
    }

    message_t__free_unpacked(pedido -> msg, NULL);

    free(pedido);

    struct data_t *ret = NULL;
    if(resposta -> msg -> opcode == MESSAGE_T__OPCODE__OP_GET+1 &&
        resposta -> msg -> c_type == MESSAGE_T__C_TYPE__CT_VALUE){
        char *data = resposta -> msg -> data;
        int data_size = resposta -> msg -> data_size;
        
        ret = data_create2(data_size, strdup(data)); //Retorna a data caso a operação seja bem sucedida
    }
    
    message_t__free_unpacked(resposta -> msg, NULL);
    free(resposta);
    
    return ret;
}

int rtree_del(struct rtree_t *rtree, char *key){
    
    struct message_t *pedido = malloc(sizeof(struct message_t));
    MessageT *msg = malloc(sizeof(MessageT));
    //Aloca memoria para as estruturas de dados
    if(msg == NULL || pedido == NULL){
        perror("Erro ao alocar memoria no rtree_del");
        return -1;
    }

    message_t__init(msg);

    msg -> opcode = MESSAGE_T__OPCODE__OP_DEL; //OPCode associado 
    msg -> c_type = MESSAGE_T__C_TYPE__CT_KEY; //C_TYPE associados
    msg -> key = key; //Key associada

    pedido -> msg = msg;
    //Envia a mensagem para o servidor e recebe a resposta do mesmo
    struct message_t *resposta = network_send_receive(rtree, pedido);

    if(resposta == NULL){
        perror("Erro ao mandar o pedido pela rede rtree_del\n");
        return -1;
    }

    message_t__free_unpacked(pedido -> msg, NULL);
    free(pedido);
    //Tendo em conta a operação ser bem ou mal sucedida
    int ret = (resposta -> msg -> opcode == MESSAGE_T__OPCODE__OP_DEL+1 && resposta -> msg -> c_type == MESSAGE_T__C_TYPE__CT_NONE) ? 0 : -1;

    message_t__free_unpacked(resposta -> msg, NULL);
    free(resposta);
    
    return ret;
}

int rtree_size(struct rtree_t *rtree){
    struct message_t *pedido = malloc(sizeof(struct message_t));
    MessageT *msg = malloc(sizeof(MessageT));
    //Aloca memoria para as estruturas de dados
    if(pedido == NULL || msg == NULL){
        perror("Erro ao alocar memoria no rtree_size");
        return -1;
    }

    message_t__init(msg);

    msg -> opcode = MESSAGE_T__OPCODE__OP_SIZE; //OPCode associado 
    msg -> c_type = MESSAGE_T__C_TYPE__CT_NONE; //C_TYPE associados
    pedido -> msg = msg;
    //Envia a mensagem para o servidor e recebe a resposta do mesmo
    struct message_t *resposta = network_send_receive(rtree, pedido);

    if(resposta == NULL){
        perror("Erro ao mandar o pedido pela rede rtree_size\n");
        return -1;
    }

    message_t__free_unpacked(msg, NULL);
    free(pedido);
    //Tendo em conta a operação ser bem ou mal sucedida
    int ret = -1;
    if(resposta -> msg -> c_type == MESSAGE_T__C_TYPE__CT_RESULT){
        ret = resposta -> msg -> tree_size; //Devolver o tamanho da árvore caso corra tudo bem!
    }  

    message_t__free_unpacked(resposta -> msg, NULL);
    free(resposta);
    return ret;
}

int rtree_height(struct rtree_t *rtree){
    struct message_t *pedido = malloc(sizeof(struct message_t));
    MessageT *msg = malloc(sizeof(MessageT));
    //Aloca memoria para as estruturas de dados
    if(pedido == NULL || msg == NULL){
        perror("Erro ao alocar memoria no rtree_height");
        return -1;
    }

    message_t__init(msg);

    msg -> opcode = MESSAGE_T__OPCODE__OP_HEIGHT; //OPCode associado 
    msg -> c_type = MESSAGE_T__C_TYPE__CT_NONE; //C_TYPE associados

    pedido -> msg = msg;
    //Envia a mensagem para o servidor e recebe a resposta do mesmo
    struct message_t *resposta = network_send_receive(rtree, pedido);

    if(resposta == NULL){
        perror("Erro ao mandar o pedido pela rede rtree_height\n");
        return -1;
    }

    message_t__free_unpacked(msg, NULL);
    free(pedido);
    //Tendo em conta a operação ser bem ou mal sucedida
    int ret = -1;
    if(resposta -> msg -> c_type == MESSAGE_T__C_TYPE__CT_RESULT){
        ret = resposta -> msg -> tree_height; //Devolver a altura da árvore caso corra tudo bem!
    } 

    message_t__free_unpacked(resposta -> msg, NULL);
    free(resposta);
    return ret;
}

char **rtree_get_keys(struct rtree_t *rtree){

    struct message_t *pedido = malloc(sizeof(struct message_t));
    MessageT *msg = malloc(sizeof(MessageT));
    //Aloca memoria para as estruturas de dados
    if(pedido == NULL || msg == NULL){
        perror("Erro ao alocar memoria no rtree_get_keys");
        return NULL;
    }

    message_t__init(msg);

    msg -> opcode = MESSAGE_T__OPCODE__OP_GETKEYS; //OPCode associado 
    msg -> c_type = MESSAGE_T__C_TYPE__CT_NONE; //C_TYPE associados

    pedido -> msg = msg;
    //Envia a mensagem para o servidor e recebe a resposta do mesmo
    struct message_t *resposta = network_send_receive(rtree, pedido);

    if(resposta == NULL) {
        perror("Erro ao mandar o pedido pela rede rtree_get_keys\n");
        return NULL;
    }

    message_t__free_unpacked(pedido -> msg, NULL);

    free(pedido);
    char **ret = NULL;
    int size = resposta -> msg -> tree_size;
    if(size > 0){
        char *keys = resposta -> msg -> key;
        ret = malloc((size + 1) * sizeof(char *));
        ret[size] = NULL;
        //Vai buscar cada uma das keys que estão na variavel resposta -> msg -> key separadas por dois pontos
        int i = 0;
        ret[i] = strdup(strtok(keys, ":"));
        i++;
        while(i < size){
            ret[i] = strdup(strtok(NULL, ":"));
            i++;
        }
    }
    message_t__free_unpacked(resposta->msg, NULL);
    free(resposta);

    return ret;
}

void rtree_free_keys(char **keys){
    int i = 0;
    while(keys[i] != NULL){
        free(keys[i]);
        i += 1;
    }
    free(keys);
}