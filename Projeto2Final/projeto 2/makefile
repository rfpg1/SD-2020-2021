# Grupo 29
# Ricardo Gonçalves fc52765
# Rafael Abrantes fc52751
# Daniel Batista fc52773

PROTOC_DIR=/usr/local/
INC_DIR = include
OBJ_DIR = object
SRC_DIR = source
BIN_DIR = binary
LIB_DIR = lib
CC = gcc
CFLAGS = -g -Wall -O2 -iquote $(INC_DIR)
LDFLAGS = ${PROTOC_DIR}$(LIB_DIR)/libprotobuf-c.a
MEMORY = valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all -v

run:
	make proto
	make treeClient
	make treeServer

server:
	./$(BIN_DIR)/treeServer 40000

client:
	./$(BIN_DIR)/treeClient 127.0.0.1:40000

%.o: $(SRC_DIR)/%.c $($@)
	$(CC) $(CFLAGS) -MD -o $(OBJ_DIR)/$@ -c $<

treeServer: tree_server.o network_server.o tree_skel.o message.o sdmessage.pb-c.o treeLib.o
	$(CC) $(addprefix $(OBJ_DIR)/, tree_server.o network_server.o tree_skel.o message.o sdmessage.pb-c.o) $(LIB_DIR)/treeLib.o $(LDFLAGS) -o $(BIN_DIR)/$@	

treeClient: tree_client.o clientLib.o
	$(CC) $(OBJ_DIR)/tree_client.o $(LIB_DIR)/clientLib.o $(LDFLAGS) -o $(BIN_DIR)/$@

proto:
	protoc --c_out=. sdmessage.proto
	mv sdmessage.pb-c.c $(SRC_DIR)/sdmessage.pb-c.c
	mv sdmessage.pb-c.h $(INC_DIR)/sdmessage.pb-c.h

clientLib.o: client_stub.o network_client.o sdmessage.pb-c.o message.o treeLib.o
	ld -r $(addprefix $(OBJ_DIR)/, client_stub.o network_client.o sdmessage.pb-c.o message.o) $(LIB_DIR)/treeLib.o -o $(LIB_DIR)/$@

treeLib.o: tree.o data.o entry.o serialization.o
	ld -r $(addprefix $(OBJ_DIR)/, $^) -o $(LIB_DIR)/$@

clean:
	rm -f $(OBJ_DIR)/*
	rm -f $(BIN_DIR)/*
	rm -f $(LIB_DIR)/* 
	rm -f $(SRC_DIR)/sdmessage.pb-c.c
	rm -f $(INC_DIR)/sdmessage.pb-c.h