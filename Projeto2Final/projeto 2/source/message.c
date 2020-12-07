/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc52751
 * Daniel Batista fc52773
**/ 

#include "message-private.h"
#include <stdlib.h>

int read_all(int socket, void* buffer, int len) {
    int tamanhoDoBuffer = len;
    int result = 0;
    
    while(len > 0){
        result = read(socket,buffer,len);
        if(result <= 0){           
            perror("Falha no read");
            return -1;
        }
        buffer += result;
        len -= result;
    }
    return tamanhoDoBuffer;
}

int write_all(int sock, void *buffer, int len) {
    if(len == 0){
      return -1;
    }
    int tamanhoDoBuffer = len;
    int result = 0;
    while(len>0) {
        result = write(sock, buffer, len);
        if(result <= 0) {
            perror("Falha no write");
            return result;
        }
        buffer += result;
        len -= result;
    }
    return tamanhoDoBuffer;
}
