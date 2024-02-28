#ifndef TREE_H
#define TREE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "table.h"
#include "utils.h"

struct Node {
  char key;
  struct Node *left;
  struct Node *right;
  char height;
  struct Table *table;
};

struct Node *createNode(char key);
struct Node *insertNode(struct Node *node, char key);
struct Node *deleteNode(struct Node *root, char key);
struct Table *getTableFromNode(struct Node *root, char key);
void freeTree(struct Node *root);
void printTree(struct Node *root);

#endif // !TREE_H
