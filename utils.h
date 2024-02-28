#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/********************************************************************
 * Constant definitions
 ********************************************************************/
#define IP_ADDRESS_LENGTH 32
#define EMPTY_DATA 0

#define MIN_OUTPUT_INTERFACE 1
#define MAX_OUTPUT_INTERFACE 0xffff

enum Error {
  OK = 0,
  REACHED_EOF = 1,

  ERROR_EMPTY_POINTER = -1,
  ERROR_MEMORY_ALLOCATION = -2,
  ERROR_WRONG_PARAMETERS = -3,
  ERROR_WRONG_PREFIX = -4,
  ERROR_WRONG_ARGS = -5,

  ERROR_TABLE_DOUBLE_INSERT = -101,
  ERROR_TABLE_MAX_ATTEMPTS = -102,
  ERROR_TABLE_NO_ENTRY = -103,

  ERROR_NODE_NOT_FOUND = -201,

  ERROR_IO_ROUTING_TABLE_NOT_FOUND = -301,
  ERROR_IO_INPUT_FILE_NOT_FOUND = -302,
  ERROR_IO_BAD_ROUTING_TABLE = -303,
  ERROR_IO_BAD_INPUT_FILE = -304,
  ERROR_IO_PARSE_ERROR = -305,
  ERROR_IO_CANNOT_CREATE_OUTPUT = -306,
  ERROR_IO_OUTPUT_INTERFACE = -307,

};

/********************************************************************
 * Generate a netmask of length prefixLength
 ********************************************************************/
void getNetmask(unsigned char prefixLength, uint32_t *netmask);

void getPrefix(uint32_t ip, char prefixLength, uint32_t *prefix);

/********************************************************************
 * Example of a very simple hash function using the modulus operator
 * For more info: https://gist.github.com/cpq/8598442
 ********************************************************************/
int hash(uint32_t IPAddress, int sizeHashTable);

char max(char a, char b);
float getSizePrefix(char prefix);

void getIPString(char **string, uint32_t ip);

enum Error raise(enum Error error);

#endif // !UTILS_H
