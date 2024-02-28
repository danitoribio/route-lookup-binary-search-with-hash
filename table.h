#ifndef TABLE_H
#define TABLE_H

#include "utils.h"

#define NUMBER_TABLES 2 // number of tables used in cucko hashing

enum EntryLabel {
  LABEL_DEFAULT = 0,
  LABEL_PREFIX = 1,
  LABEL_MARKER = 2,
  LABEL_PREFIX_AND_MARKER = 3,
};

struct Entry {
  uint32_t key;
  enum EntryLabel label;
  unsigned short data;
};

struct Table {
  char prefix;
  unsigned int size;
  struct Entry *entries[NUMBER_TABLES];
  uint32_t (*hashFuntion[NUMBER_TABLES])(uint32_t key, unsigned int table_size);
};

struct Table *createTable(char prefix);
char insertData(struct Table *table, uint32_t ip, enum EntryLabel label,
                unsigned short data);
char deleteData(struct Table *table, uint32_t ip);
struct Entry *findEntry(struct Table *table, uint32_t ip);
void freeTable(struct Table *table);
void printTable(struct Table *table);

#endif // !TABLE_H
