#include <ctype.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MMAP(pointer, size)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  \
  { (pointer) = mmap(NULL, (size), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); }
#define UNMAP(pointer, size)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 \
  { munmap((pointer), (size)); }

enum argumentErrors {argCount, argPositiveNum, argRange, memError, fileError};

typedef struct {
  sem_t waiting;
  sem_t boarded;
  sem_t mutex;
  unsigned int skiersWaiting;
  unsigned int skiing;
} stop_t;

typedef struct shared {
    stop_t *busStop;
    sem_t printLock;
    sem_t waitFinal;
    sem_t wentSkiing;
    size_t lines;
    FILE *file;
    unsigned int skierAmount;
    unsigned int busCapacity;
    unsigned int stopAmount;
    unsigned int onBoard;
    unsigned int skiersSkiing;
} shared_t;

bool inRange(int min, int max, int value);
void parseArgs(int argc, char *argv[]);
void errorMessage(int errType);
void initSem(shared_t *shared);
void destSem(shared_t *shared);
void printFile(shared_t *shared, char *object, ...);
void bus(shared_t *shared, int TB);
void skier(shared_t *shared, int id, int TL);
