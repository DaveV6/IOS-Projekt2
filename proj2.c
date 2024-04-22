#include "proj2.h"

enum{argCount, argPositiveNum, argRange, memError};

bool inRange(int min, int max, int value) {
    return (value >= min && value <= max);
}

void parseArgs(int argc, char *argv[]) {
    if(argc != 6) {
        errorMessage(argCount);
    }

    for(int i = 1; i < argc; i++) {
        char *endptr;
        int arg = strtol(argv[i], &endptr, 0);
        if(*endptr != '\0' || arg < 0) {
            errorMessage(argPositiveNum);
        }
    }
}

void errorMessage(int errType) {
    switch(errType) {
        case argCount:
            fprintf(stderr, "ERROR: The number of arguments must be 6\n");
            break;
        case argPositiveNum:
            fprintf(stderr, "ERROR: Arguments must be a positive integer!\n");
            break;
        case argRange:
            fprintf(stderr, "ERROR: The values of the arguments must be in the allowed range\n");
            break;
        case memError:
            fprintf(stderr, "ERROR: A memory error has occured.\n");
            break;
    }

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {

    parseArgs(argc, argv);

    int L = strtol(argv[1], NULL, 10);
    int Z = strtol(argv[2], NULL, 10);
    int K = strtol(argv[3], NULL, 10);
    int TL = strtol(argv[4], NULL, 10);
    int TB = strtol(argv[5], NULL, 10);

    if(Z > 10 || Z == 0 || !inRange(10, 100, K) || !inRange(0, 10000, TL) || !inRange(0, 1000, TB)) {
        errorMessage(argRange);
    }

    printf("L: %d\n", L);
    printf("Z: %d\n", Z);
    printf("K: %d\n", K);
    printf("TL: %d\n", TL);
    printf("TB: %d\n", TB);

    exit(EXIT_SUCCESS);
}