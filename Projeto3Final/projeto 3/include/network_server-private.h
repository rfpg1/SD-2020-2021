/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc52751
 * Daniel Batista fc52773
**/


#ifndef _NETWORK_SERVER_PRIVATE_H
#define _NETWORK_SERVER_PRIVATE_H

/* Signal Handler for SIGINT */
void terminaServidor();
struct pollfd *realocar(struct pollfd *connections, int capacidade, int sinal);
#endif