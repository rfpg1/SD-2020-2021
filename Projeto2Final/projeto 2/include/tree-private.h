/**
Ricardo Gonçalves fc52765
Rafael Abrantes fc52751
Daniel Batista fc52773
Grupo 29
*/
#ifndef _TREE_PRIVATE_H
#define _TREE_PRIVATE_H

#include "tree.h"

struct tree_t{
	struct entry_t *value;
	struct tree_t *nodeLeft;
	struct tree_t *nodeRight;
	int size;
};

void tree_insert (struct tree_t *tree, struct  entry_t *entry);

struct tree_t *tree_delete(struct tree_t *tree, char *key);

struct tree_t *minValueNode (struct tree_t *tree);

char **tree_get_chaves(struct tree_t *tree, char **str, int i);

#endif
