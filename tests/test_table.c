#include "../table.h"
#include "miniunit.h"
#include <stdint.h>

/*
 * Wanted result
 * Printing table 0
 * 	L:1 K:560 BMP:-1 D:8
 * 	L:1 K:43690 BMP:-1 D:4
 * 	L:1 K:65441 BMP:-1 D:0
 * 	L:1 K:8249 BMP:-1 D:7
 * 	L:1 K:771 BMP:-1 D:2
 * 	L:1 K:8960 BMP:-1 D:3
 * 	L:1 K:4660 BMP:-1 D:9
 * Printing table 1
 * 	L:1 K:561 BMP:-1 D:5
 * 	L:1 K:1172 BMP:-1 D:6
 * 	L:1 K:769 BMP:-1 D:1
 */
static char testCreateTable() {
  struct Table *table = createTable(16);
  if (table == NULL) {
    printf("error\n");
    return -1;
  }

  uint32_t ip[11] = {0x0000, 0x0301, 0x0303, 0x2300, 0xAAAA, 0x0231,
                     0x0494, 0x2039, 0x0230, 0x1234, 0xAAAA};

  for (unsigned char i = 0; i < 11; i++) {
    insertData(table, ip[i], LABEL_PREFIX, i);
  }
  printTable(table);

  freeTable(table);

  return 0;
}

void testTable() {
  test_case("TABLE", { expect("create table", testCreateTable() == 0); });
}
