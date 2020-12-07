#Ricardo Gonçalves fc52765	
#Rafael Abrantes fc52751
#Daniel Batista fc52773
#Grupo 29
SRC = source
OBJ= object
CC = gcc
H = include/
BIN = binary
CFLAGS = -g -Wall -I $(H)
MEMORY = valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all -v
OBJECTOS = data.o test_data.o entry.o test_entry.o tree.o test_tree.o serialization.o test_serialization.o


test_data: data.o test_data.o
	$(CC) $(addprefix $(OBJ)/,data.o test_data.o) -o $(BIN)/$@

test_entry: data.o entry.o test_entry.o
	$(CC) $(addprefix $(OBJ)/,data.o test_entry.o entry.o) -o $(BIN)/$@

test_tree: data.o entry.o tree.o test_tree.o
	$(CC) $(addprefix $(OBJ)/,data.o entry.o tree.o test_tree.o) -o $(BIN)/$@

test_serialization: data.o entry.o serialization.o test_serialization.o tree.o
	$(CC) $(addprefix $(OBJ)/,data.o entry.o serialization.o test_serialization.o tree.o) -o $(BIN)/$@

%.o: $(SRC)/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ)/$@ -c $<

run:
	make tests
	make runmemoria

tests:
	make test_data
	-./$(BIN)/test_data
	make test_entry
	-./$(BIN)/test_entry
	make test_tree
	-./$(BIN)/test_tree
	make test_serialization
	./$(BIN)/test_serialization

runmemoria:
	-$(MEMORY) ./$(BIN)/test_data
	-$(MEMORY) ./$(BIN)/test_entry
	-$(MEMORY) ./$(BIN)/test_tree
	-$(MEMORY) ./$(BIN)/test_serialization
mem_data:
	-$(MEMORY) ./$(BIN)/test_data

mem_entry:
	-$(MEMORY) ./$(BIN)/test_entry

mem_tree:
	-$(MEMORY) ./$(BIN)/test_tree
mem_serialization:
	-$(MEMORY) ./$(BIN)/test_serialization
clean:
	rm -f $(addprefix $(OBJ)/,$(OBJECTOS))
	rm -f $(BIN)/test_data
	rm -f $(BIN)/test_entry
	rm -f $(BIN)/test_tree
	rm -f $(BIN)/test_serialization


