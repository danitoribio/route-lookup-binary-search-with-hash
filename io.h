#ifndef IO_H
#define IO_H

#include <sys/resource.h>
#include <sys/time.h>

#include "utils.h"

/***********************************************************************
 * Write the input to the specified file (f) and the standard output
 *
 * Use as fprintf(FILE *stream, const char *format, ...)
 *
 ***********************************************************************/
void tee(FILE *f, char const *fmt, ...);

/***********************************************************************
 * Initalize file descriptors
 *
 * routingTableName contains FIB info (argv[1] of main function)
 * inputFileName contains IP addresses (argv[2] of main function)
 *
 ***********************************************************************/
char initializeIO(char *routingTableName, char *inputFileName);
void resetIO();
/***********************************************************************
 * Close the input/output files
 ***********************************************************************/
void freeIO();

/***********************************************************************
 * Read one entry in the FIB
 *
 * It should be noted that prefix, prefixLength and outInterface are
 * pointers since they are used as output parameters
 *
 ***********************************************************************/
char readFIBLine(uint32_t *prefix, int *prefixLength, int *outInterface);

/***********************************************************************
 * Read one entry in the input packet file
 *
 * Again, it should be noted that IPAddress is a pointer since it is used
 * as output parameter
 *
 ***********************************************************************/
char readInputPacketFileLine(uint32_t *IPAddress);

/***********************************************************************
 * Print a line to the output file
 *
 * clock_gettime(CLOCK_MONOTONIC_RAW, &initialTime) must be called right before
 *the lookup function
 *
 * clock_gettime(CLOCK_MONOTONIC_RAW, &finalTime) must be called right after the
 *lookup function
 *
 * The lookup function must return (either as output parameter or as return
 *value) the number of hash tables that have been accessed for every IP address
 *
 ***********************************************************************/
void printOutputLine(uint32_t IPAddress, int outInterface,
                     struct timespec *initialTime, struct timespec *finalTime,
                     double *searchingTime, int numberOfTableAccesses);

/***********************************************************************
 * Print execution summary to the output file
 *
 * It should be noted that:
 *
 * 		averageTableAccesses = totalTableAccesses/processedPackets
 *
 *		averagePacketProcessingTime =
 *totalPacketProcessingTime/processedPackets
 *
 ***********************************************************************/
void printSummary(int processedPackets, double averageTableAccesses,
                  double averagePacketProcessingTime);

/***********************************************************************
 * Print memory and CPU time
 *
 * For more info: man getrusage
 *
 ***********************************************************************/
void printMemoryTimeUsage();

#endif // !IO_H
