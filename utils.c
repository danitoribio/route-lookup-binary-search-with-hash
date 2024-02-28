#include "utils.h"

/********************************************************************
 * Generate a netmask of length prefixLength
 ********************************************************************/
void getNetmask(unsigned char prefixLength, uint32_t *netmask) {

  *netmask = (0xFFFFFFFF << (IP_ADDRESS_LENGTH - prefixLength));
}

void getPrefix(uint32_t ip, char prefixLength, uint32_t *prefix) {
  if ((prefixLength < 0) || (prefixLength > IP_ADDRESS_LENGTH)) {
    *prefix = 0;
    raise(ERROR_WRONG_PREFIX);
    return;
  }

  if (prefixLength == 0) {
    *prefix = 0;
    return;
  }

  uint32_t netmask = 0;
  getNetmask(prefixLength, &netmask);

  *prefix = ip & netmask;
}

/*
 * Return the max of two integers
 */
char max(char a, char b) { return (a > b) ? a : b; }

float getSizePrefix(char prefix) { return (powf(2, prefix)); }

void getIPString(char **string, uint32_t ip) {
  sprintf(*string, "%u.%u.%u.%u", ip >> 24, (ip >> 16) & 0xff, (ip >> 8) & 0xff,
          ip & 0xff);
}

static void printError(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  printf("\n!!!!!!!!!!!!!!! ERROR: ");
  printf(format, ap);
  printf(" !!!!!!!!!!!!!!!\n\n");
  va_end(ap);
}

enum Error raise(enum Error error) {
  switch (error) {
  case OK:
    break;
  case REACHED_EOF:
    break;
  case ERROR_EMPTY_POINTER:
    printError("Empty pointer detected");
    break;
  case ERROR_MEMORY_ALLOCATION:
    printError("Memory allocation error");
    break;
  case ERROR_WRONG_PARAMETERS:
    printError("The parameters of the functions are not correct");
    break;
  case ERROR_WRONG_PREFIX:
    printError("The prefix given is hiehger than 32 or lower than 0");
    break;
  case ERROR_WRONG_ARGS:
    printError("Please pass the fib file and the input files as arguments");
    break;

  case ERROR_TABLE_DOUBLE_INSERT:
    printError("Trying to insert a key in a table that is already there");
    break;
  case ERROR_TABLE_MAX_ATTEMPTS:
    printError("Max attempts reached inserting a key");
    break;
  case ERROR_TABLE_NO_ENTRY:
    printError("No entry in the given table");
    break;

  case ERROR_NODE_NOT_FOUND:
    printError("Not node found with given key");
    break;

  case ERROR_IO_ROUTING_TABLE_NOT_FOUND:
    printError("The routing table file could not be found");
    break;
  case ERROR_IO_INPUT_FILE_NOT_FOUND:
    printError("The input file could not be found");
    break;
  case ERROR_IO_BAD_ROUTING_TABLE:
    printError("The routing table has a bad format");
    break;
  case ERROR_IO_BAD_INPUT_FILE:
    printError("The input file has a bad format");
    break;
  case ERROR_IO_PARSE_ERROR:
    printError("There was an error parsing the files");
    break;
  case ERROR_IO_CANNOT_CREATE_OUTPUT:
    printError("Could not create the output file");
    break;
  case ERROR_IO_OUTPUT_INTERFACE:
    printError("The output interface does not fit in data");
    break;
  }

  return error;
}
