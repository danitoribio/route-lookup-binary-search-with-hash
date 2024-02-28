#include "table.h"
#include "utils.h"

/* #define TABLE_SIZE_DEFAULT 5000 // number of slots in each table */
#define TABLE_SIZE_DEFAULT 10000 // number of slots in each table
#define MAX_ATTEMPTS 50          // maximum number of attempts to insert a key

// TODO try to optimize this functions with pointers
static uint32_t hash1(uint32_t key, unsigned int table_size) {
  uint32_t hash = key;
  hash ^= key;
  hash *= 0x1835a82b;
  hash ^= key >> 8;
  hash *= 0x85ebca6b;
  hash ^= key >> 16;
  hash *= 0xc2b2ae35;
  hash ^= key >> 24;
  return hash % table_size;
}

static uint32_t hash2(uint32_t key, unsigned int table_size) {
  uint32_t hash = key;
  hash += key >> 24;
  hash -= 0x0193a48b;
  hash += key >> 16;
  hash -= 0x7a11c3e5;
  hash += key >> 8;
  hash -= 0x9f1a0306;
  hash += key;
  return hash % table_size;
}

struct Table *createTable(char prefix) {
  struct Table *table = (struct Table *)malloc(sizeof(struct Table));
  if (table == NULL) {
    raise(ERROR_MEMORY_ALLOCATION);
    return NULL;
  }

  for (unsigned char i = 0; i < NUMBER_TABLES; i++) {
    table->entries[i] = NULL;
  }

  table->size = TABLE_SIZE_DEFAULT;

  for (unsigned char i = 0; i < NUMBER_TABLES; i++) {
    table->entries[i] =
        (struct Entry *)calloc(table->size, sizeof(struct Entry));
    if (table->entries[i] == NULL) {
      freeTable(table);
      raise(ERROR_MEMORY_ALLOCATION);
      return NULL;
    }

    table->entries[i]->key = -1;
    table->entries[i]->label = LABEL_DEFAULT;
    table->entries[i]->data = EMPTY_DATA;
  }

  table->prefix = prefix;
  table->hashFuntion[0] = hash1;
  table->hashFuntion[1] = hash2;

  return table;
}

// TODO when max attempt is reached resize the table
char insertData(struct Table *table, uint32_t ip, enum EntryLabel label,
                unsigned short data) {
  if (table == NULL) {
    return raise(ERROR_EMPTY_POINTER);
  }

  uint32_t key;
  getPrefix(ip, table->prefix, &key);

  for (unsigned char i = 0; i < NUMBER_TABLES; i++) {
    uint32_t hash = table->hashFuntion[i](key, table->size);

    if ((table->entries[i][hash].key == key) &&
        (table->entries[i][hash].label != LABEL_DEFAULT)) {

      if ((table->entries[i][hash].label == LABEL_PREFIX) &&
          (label == LABEL_MARKER)) {
        table->entries[i][hash].label = LABEL_PREFIX_AND_MARKER;
      }
      return ERROR_TABLE_DOUBLE_INSERT;
    }
  }

  for (unsigned char i = 0; i < MAX_ATTEMPTS; i++) {
    unsigned char index = i % NUMBER_TABLES;
    uint32_t hash = table->hashFuntion[index](key, table->size);

    if (table->entries[index][hash].label == LABEL_DEFAULT) {
      table->entries[index][hash].key = key;
      table->entries[index][hash].label = label;
      table->entries[index][hash].data = data;

      return OK;
    } else {
      uint32_t tmp_key = table->entries[index][hash].key;
      enum EntryLabel tmp_label = table->entries[index][hash].label;
      unsigned short tmp_data = table->entries[index][hash].data;

      table->entries[index][hash].key = key;
      table->entries[index][hash].label = label;
      table->entries[index][hash].data = data;

      key = tmp_key;
      label = tmp_label;
      data = tmp_data;
    }
  }

  char *ip_string = malloc(16 * sizeof(char));
  getIPString(&ip_string, key);
  printf("Prefix: %u Key: %s\n", table->prefix, ip_string);
  free(ip_string);
  return raise(ERROR_TABLE_MAX_ATTEMPTS);
}

char deleteData(struct Table *table, uint32_t ip) {
  if (table == NULL) {
    return raise(ERROR_EMPTY_POINTER);
  }

  uint32_t key;
  getPrefix(ip, table->prefix, &key);

  struct Entry *entry = findEntry(table, key);
  if (entry == NULL) {
    return raise(ERROR_TABLE_NO_ENTRY);
  }

  entry->key = -1;
  entry->label = LABEL_DEFAULT;
  entry->data = EMPTY_DATA;

  return OK;
}

struct Entry *findEntry(struct Table *table, uint32_t ip) {
  if (table == NULL) {
    raise(ERROR_EMPTY_POINTER);
    return NULL;
  }

  uint32_t key;
  getPrefix(ip, table->prefix, &key);

  for (unsigned char i = 0; i < NUMBER_TABLES; i++) {
    uint32_t hash = table->hashFuntion[i](key, table->size);
    if ((table->entries[i][hash].key == key) &&
        (table->entries[i][hash].label != LABEL_DEFAULT)) {
      return &(table->entries[i][hash]);
    }
  }

  return NULL;
}

void freeTable(struct Table *table) {
  if (table == NULL) {
    return;
  }

  for (unsigned char i = 0; i < NUMBER_TABLES; i++) {
    if (table->entries[i] != NULL) {
      free(table->entries[i]);
    }
  }

  free(table);
}

void printTable(struct Table *table) {
  if (table == NULL) {
    return;
  }

  for (unsigned char i = 0; i < NUMBER_TABLES; i++) {
    printf("\tPrinting table %u\n", i);

    if (table->entries[i] == NULL) {
      continue;
    }

    struct Entry *entry_array = table->entries[i];

    for (unsigned int j = 0; j < table->size; j++) {
      if (entry_array[j].label != LABEL_DEFAULT) {
        char *label = entry_array[j].label == LABEL_PREFIX   ? "P"
                      : entry_array[j].label == LABEL_MARKER ? "M"
                                                             : "P&M";

        char *ip_string = malloc(16 * sizeof(char));
        getIPString(&ip_string, entry_array[j].key);

        printf("\t\tL:%s K:%s D:%d\n", label, ip_string, entry_array[j].data);

        free(ip_string);
      }
    }
  }
}
