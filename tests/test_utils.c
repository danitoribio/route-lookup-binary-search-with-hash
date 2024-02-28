#include "../utils.h"

#include "miniunit.h"

#define IP_NUMBER 11

static char testGetPrefixFromIP() {
  uint32_t ip[IP_NUMBER] = {0xF0000000, 0xFF000000, 0xF303, 0xA300,
                            0xAAAA,     0x0231,     0x0494, 0x2039,
                            0x0230,     0x1234,     0xAAAA};

  char *string_ip = malloc(16 * sizeof(char));
  char *string_prefix = malloc(16 * sizeof(char));

  for (unsigned char i = 0; i < IP_NUMBER; i++) {
    uint32_t prefix = 0;
    getPrefix(ip[i], 6, &prefix);

    getIPString(&string_ip, ip[i]);
    getIPString(&string_prefix, prefix);
    printf("IP:%s Prefix:%s \n", string_ip, string_prefix);
  }

  free(string_ip);
  free(string_prefix);

  return 0;
}

static char testAllPrefixesFromIP() {
  uint32_t ip = 0xFFFFFFFF;

  char *string_ip = malloc(16 * sizeof(char));
  char *string_prefix = malloc(16 * sizeof(char));

  for (unsigned char i = 0; i < 33; i++) {
    uint32_t prefix = 0;
    getPrefix(ip, i, &prefix);

    getIPString(&string_ip, ip);
    getIPString(&string_prefix, prefix);
    printf("Netmask:%u IP:%s Prefix:%s \n", i, string_ip, string_prefix);
  }

  free(string_ip);
  free(string_prefix);

  return 0;
}

void testUtils() {
  test_case("UTILS",
            { expect("Check prefix form ip", testAllPrefixesFromIP() == 0); });
}
