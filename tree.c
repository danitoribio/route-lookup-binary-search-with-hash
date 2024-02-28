#include "tree.h"
#include "table.h"
#include "utils.h"

/*
 * Returns the height of the node
 */
static char getHeight(struct Node *node) {
  if (node == NULL) {
    return 0;
  }

  return node->height;
}

/*
 * Updates the maxiumum height of the tree adding one
 */
static char updateMaxHeightPlusOne(struct Node *node) {
  if (node == NULL) {
    return raise(ERROR_EMPTY_POINTER);
  }

  node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
  return OK;
}

/*
 * Returns the balance factor of a node
 */
static char getBalanceFactor(struct Node *node) {
  if (node == NULL) {
    return raise(ERROR_EMPTY_POINTER);
  }

  return getHeight(node->left) - getHeight(node->right);
}

/*
 * Computes the minimum value node, that is the most left node
 */
static struct Node *getMinValueNode(struct Node *node) {
  if (node == NULL) {
    raise(ERROR_EMPTY_POINTER);
    return NULL;
  }

  struct Node *current = node;

  while (current->left != NULL)
    current = current->left;

  return current;
}

/*
 * Rotates to the right the subtree rooted at x
 *     x           --- y
 *     |           |
 *     --- y  =>   x ---
 *         |           |
 *     z ---           z
 */
static struct Node *rotateLeft(struct Node *x) {
  if (x == NULL) {
    raise(ERROR_EMPTY_POINTER);
    return NULL;
  }

  struct Node *y = x->right;
  struct Node *z = y->left;

  y->left = x;
  x->right = z;

  updateMaxHeightPlusOne(x);
  updateMaxHeightPlusOne(y);

  return y;
}

/*
 * Rotates to the right the subtree rooted at y
 *     --- y        x ---
 *     |                |
 *     x ---   =>   --- y
 *         |        |
 *         z        z
 */
static struct Node *rotateRight(struct Node *y) {
  if (y == NULL) {
    raise(ERROR_EMPTY_POINTER);
    return NULL;
  }

  struct Node *x = y->left;
  struct Node *z = x->right;

  x->right = y;
  y->left = z;

  updateMaxHeightPlusOne(y);
  updateMaxHeightPlusOne(x);

  return x;
}

static void freeNode(struct Node *node) {
  if (node == NULL) {
    return;
  }

  freeTable(node->table);
  free(node);
}

/*
 * Creates a new node given the table
 */
struct Node *createNode(char key) {
  if (key < 0) {
    raise(ERROR_WRONG_PARAMETERS);
    return NULL;
  }
  struct Node *node = (struct Node *)malloc(sizeof(struct Node));
  if (node == NULL) {
    raise(ERROR_MEMORY_ALLOCATION);
    return NULL;
  }

  if ((node->table = createTable(key)) == NULL) {
    raise(ERROR_MEMORY_ALLOCATION);
    freeNode(node);
    return NULL;
  }

  node->key = key;
  node->left = NULL;
  node->right = NULL;
  node->height = 1;

  return node;
}

/*
 * Inserts a node with a specific key
 */
struct Node *insertNode(struct Node *node, char key) {
  if (node == NULL) {
    return createNode(key);
  }

  if (key < node->key) {
    node->left = insertNode(node->left, key);
  } else if (key > node->key) {
    node->right = insertNode(node->right, key);
  } else {
    return node;
  }

  // Update the height of the new node
  updateMaxHeightPlusOne(node);

  // Balance the tree
  char balance = getBalanceFactor(node);
  if ((balance > 1) && (key < node->left->key)) {
    return rotateRight(node);
  }

  if ((balance < -1) && (key > node->right->key)) {
    return rotateLeft(node);
  }

  if ((balance > 1) && (key > node->left->key)) {
    node->left = rotateLeft(node);
    return rotateRight(node);
  }

  if ((balance < -1) && (key < node->right->key)) {
    node->right = rotateRight(node->right);
    return rotateLeft(node);
  }

  return node;
}

/*
 * Deletes a node with a specific key
 */
struct Node *deleteNode(struct Node *root, char key) {
  // Find the node and delete it
  if (root == NULL) {
    raise(ERROR_EMPTY_POINTER);
    return NULL;
  }

  if (key < root->key) {
    root->left = deleteNode(root->left, key);
  } else if (key > root->key) {
    root->right = deleteNode(root->right, key);
  } else {
    if ((root->left == NULL) || (root->right == NULL)) {
      struct Node *temp = root->left ? root->left : root->right;

      if (temp == NULL) {
        temp = root;
        root = NULL;
      } else {
        *root = *temp;
      }

      freeNode(temp);
    } else {
      struct Node *temp = getMinValueNode(root->right);
      root->key = temp->key;
      root->right = deleteNode(root->right, temp->key);
    }
  }

  if (root == NULL) {
    return root;
  }

  // Update the height of the root
  updateMaxHeightPlusOne(root);

  // Balance the tree
  char balance = getBalanceFactor(root);
  if (balance > 1 && getBalanceFactor(root->left) >= 0) {
    return rotateRight(root);
  }

  if (balance > 1 && getBalanceFactor(root->left) < 0) {
    root->left = rotateLeft(root->left);
    return rotateRight(root);
  }

  if (balance < -1 && getBalanceFactor(root->right) <= 0) {
    return rotateLeft(root);
  }

  if (balance < -1 && getBalanceFactor(root->right) > 0) {
    root->right = rotateRight(root->right);
    return rotateLeft(root);
  }

  return root;
}

struct Table *getTableFromNode(struct Node *root, char key) {
  if (root == NULL) {
    raise(ERROR_NODE_NOT_FOUND);
    return NULL;
  }

  if (root->key == key) {
    return root->table;
  } else if (key < root->key) {
    return getTableFromNode(root->left, key);
  } else if (key > root->key) {
    return getTableFromNode(root->right, key);
  }

  return NULL;
}

/*
 * Frees all the tree recursivly
 */
void freeTree(struct Node *root) {
  if (root == NULL) {
    return;
  }

  freeTree(root->right);
  freeTree(root->left);
  freeNode(root);
}

/*
 * Print the node and its subtree with padding and identifier
 */
static void printNode(struct Node *node, char space, char identifier) {
  if (node == NULL) {
    return;
  }
  space += 1;
  printNode(node->right, space, 'R');

  for (int i = 0; i < space; i++) {
    printf("\t");
  }
  printf("%d%c\n", node->key, identifier);
  printNode(node->left, space, 'L');
}

static void printTreeTables(struct Node *node) {
  if (node == NULL) {
    return;
  }

  printTreeTables(node->left);

  printf("\n PREFIX %u\n", node->key);
  printTable(node->table);

  printTreeTables(node->right);
}

/*
 * Print the tree rooted at the node root
 */
void printTree(struct Node *root) {
  printNode(root, 0, 'M');
  printTreeTables(root);
}
