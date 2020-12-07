/**
 * Grupo 29
 * Ricardo Gonçalves fc52765
 * Rafael Abrantes fc52751
 * Daniel Batista fc52773
**/

--Makefile

run: compila o ficheiro proto e gera os executáveis para o treeServer e treeClient

server: corre o treeServer com os argumentos necessários

client: corre o treeClient com os argumentos necessários

proto: compila o ficheiro proto, e movo o .h para a pasta include e o .c para a pasta source

clean: limpa todos os executaveis, objetos e bibliotecas

--getkeys

Optamos por serializar manualmente a lista de keys de forma a termos
mais flexibilidade. Como as keys não podem conter caracteres de dois pontos, metemos
as keys todas numa string separadas por dois pontos, ao serializar.

--protobuf

Adicionamos parametros extra na nossa mensagem para tornar a rececao de
dados mais coerente, defenindo campos expessificos para certos datos
(key, data, data_size...).