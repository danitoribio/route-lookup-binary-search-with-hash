#include "main.h"
#include "io.h"
#include "table.h"
#include "tree.h"
#include "utils.h"

#include <time.h>

static struct Node *generateTree() {
  uint32_t prefix;
  int prefixLength, outInterface;

  resetIO();
  struct Node *root = NULL;

  while (readFIBLine(&prefix, &prefixLength, &outInterface) != REACHED_EOF) {
    root = insertNode(root, prefixLength);
  }

  return root;
}

static void fillTreeWithPrefixes(struct Node *root) {
  if (root == NULL) {
    raise(ERROR_NODE_NOT_FOUND);
    return;
  }

  resetIO();

  uint32_t ip;
  int prefixLength, outInterface;
  int previousPrefixLength = -1;
  struct Table *table = NULL;

  while (readFIBLine(&ip, &prefixLength, &outInterface) != REACHED_EOF) {
    if ((outInterface < MIN_OUTPUT_INTERFACE) ||
        (outInterface > MAX_OUTPUT_INTERFACE)) {
      raise(ERROR_IO_OUTPUT_INTERFACE);
      continue;
    }
    if (prefixLength != previousPrefixLength) {
      table = getTableFromNode(root, prefixLength);
    }

    insertData(table, ip, LABEL_PREFIX, outInterface);
  }
}

static void addMarkersFromTableToNode(struct Node *node, struct Table *table) {
  if ((node == NULL) || (table == NULL)) {
    raise(ERROR_EMPTY_POINTER);
    return;
  }

  for (unsigned char i = 0; i < NUMBER_TABLES; i++) {
    if (table->entries[i] == NULL) {
      continue;
    }

    struct Entry *entry_array = table->entries[i];

    for (unsigned int j = 0; j < table->size; j++) {
      if (entry_array[j].label != LABEL_DEFAULT) {
        insertData(node->table, entry_array[j].key, LABEL_MARKER, EMPTY_DATA);
      }
    }
  }
}

static void computeMarkersForSubtree(struct Node *node) {
  if (node == NULL) {
    return;
  }

  if (node->right == NULL) {
    return;
  }

  computeMarkersForSubtree(node->right);
  addMarkersFromTableToNode(node, node->right->table);

  struct Node *left_node = node->right->left;
  while (left_node != NULL) {
    computeMarkersForSubtree(left_node);
    addMarkersFromTableToNode(node, left_node->table);
    left_node = left_node->left;
  }
}

static void fillTreeWithMarkers(struct Node *root) {
  if (root == NULL) {
    raise(ERROR_EMPTY_POINTER);
    return;
  }

  do {
    computeMarkersForSubtree(root);
  } while ((root = root->left) != NULL);
}

static unsigned short findBestMatchingPrefix(struct Node *root, uint32_t ip,
                                             unsigned char prefix) {
  if (root == NULL) {
    raise(ERROR_EMPTY_POINTER);
    return EMPTY_DATA;
  }

  unsigned short lmp = EMPTY_DATA;
  struct Entry *entry = NULL;
  do {
    if (root->key >= prefix) {
      root = root->left;
      continue;
    }

    entry = findEntry(root->table, ip);
    if (entry == NULL) {
      root = root->left;
    } else {
      lmp = entry->data;
      root = root->right;
    }
  } while (root != NULL);

  return lmp;
}

static void addBMPToTable(struct Table *table, struct Node *root) {
  if ((root == NULL) || (table == NULL)) {
    raise(ERROR_EMPTY_POINTER);
    return;
  }

  for (unsigned char i = 0; i < NUMBER_TABLES; i++) {
    if (table->entries[i] == NULL) {
      continue;
    }

    struct Entry *entry_array = table->entries[i];

    for (unsigned int j = 0; j < table->size; j++) {
      if ((entry_array[j].label == LABEL_MARKER) &&
          (entry_array[j].data == EMPTY_DATA)) {
        unsigned short lmp =
            findBestMatchingPrefix(root, entry_array[j].key, table->prefix);
        if (lmp != EMPTY_DATA) {
          entry_array[j].data = lmp;
        }
      }
    }
  }
}

static void computeBMPForSubtree(struct Node *node, struct Node *root) {
  if (node == NULL) {
    return;
  }

  computeBMPForSubtree(node->left, root);

  addBMPToTable(node->table, root);

  computeBMPForSubtree(node->right, root);
}

static void fillTreeWithBmp(struct Node *root) {
  if (root == NULL) {
    raise(ERROR_EMPTY_POINTER);
    return;
  }

  computeBMPForSubtree(root, root);
}

static void findLongestMatchingPrefix(struct Node *root, uint32_t ip,
                                      unsigned short *outInterface,
                                      unsigned char *tableAccesses) {
  struct Entry *entry = NULL;
  do {
    entry = findEntry(root->table, ip);
    (*tableAccesses)++;
    if (entry == NULL) {
      root = root->left;
    } else {
      *outInterface = entry->data;
      if (entry->label == LABEL_PREFIX) {
        return;
      }

      root = root->right;
    }
  } while (root != NULL);
}

static void computeLMPForInputPakcetFile(struct Node *root) {
  if (root == NULL) {
    raise(ERROR_NODE_NOT_FOUND);
    return;
  }

  resetIO();

  uint32_t ip;
  unsigned short outInterface;
  unsigned char tableAccesses;
  struct timespec initialTime, finalTime;
  double searchTime, totalTime = 0;
  unsigned int processedPackets = 0, totalTableAccesses = 0;

  while (readInputPacketFileLine(&ip) != REACHED_EOF) {
    outInterface = EMPTY_DATA;
    tableAccesses = 0;

    clock_gettime(CLOCK_MONOTONIC_RAW, &initialTime);
    findLongestMatchingPrefix(root, ip, &outInterface, &tableAccesses);
    clock_gettime(CLOCK_MONOTONIC_RAW, &finalTime);

    printOutputLine(ip, outInterface, &initialTime, &finalTime, &searchTime,
                    tableAccesses);
    totalTime += searchTime;
    totalTableAccesses += tableAccesses;
    processedPackets++;
  }

  printSummary(processedPackets,
               ((double)totalTableAccesses / processedPackets),
               (totalTime / processedPackets));
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return raise(ERROR_WRONG_ARGS);
  }
  char *fib = argv[1];
  char *input = argv[2];

  initializeIO(fib, input);

  struct Node *root = generateTree();
  if (root == NULL) {
    return raise(ERROR_EMPTY_POINTER);
  }

  fillTreeWithPrefixes(root);
  fillTreeWithMarkers(root);
  fillTreeWithBmp(root);

  computeLMPForInputPakcetFile(root);

  freeTree(root);

  freeIO();

  return OK;
}
