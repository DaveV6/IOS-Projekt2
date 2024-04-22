#include <ctype.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if(argc != 5) {
        fprintf(stderr, "ERROR: The number of arguments must be 5\n");
        exit(EXIT_FAILURE);
    }

    int L = strtol(argv[1], NULL, 10); // number of skiers
    int Z = strtol(argv[2], NULL, 10); // amount of stops
    int K = strtol(argv[3], NULL, 10); // capacity
    int TL = strtol(argv[4], NULL, 10); // max waiting time in microseconds while he waits for the bus
    int TB = strtol(argv[5], NULL, 10); // max time between two bus stops

    if(L >= 20000) {
        fprintf(stderr, "ERROR: The number of skiers must be less than 20000\n");
        exit(EXIT_FAILURE);
    }

    if(Z <= 0 || Z > 10) {
        fprintf(stderr, "ERROR: The number of stops must be between 1 and 10\n");
        exit(EXIT_FAILURE);
    }

    if(K < 10 || K > 100) {
        fprintf(stderr, "ERROR: The capacity of the bus must be between 10 and 100\n");
        exit(EXIT_FAILURE);
    }

    if(TL < 0 || TL > 10000) {
        fprintf(stderr, "ERROR: The max waiting time must be between 0 and 10000\n");
        exit(EXIT_FAILURE);
    }

    if(TB < 0 || TB > 1000) {
        fprintf(stderr, "ERROR: The max time between two bus stops must be between 0 and 1000\n");
        exit(EXIT_FAILURE);
    }

}