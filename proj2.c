#include "proj2.h"

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
        case fileError:
            fprintf(stderr, "ERROR: Failed to open and write in the file.\n");
            break;
    }

    exit(EXIT_FAILURE);
}

bool inRange(int min, int max, int value) {
    return (value >= min && value <= max);
}

void cleanMem(shared_t *shared) { munmap(shared, sizeof(shared_t)); }

void initSem(shared_t *shared) {
    sem_init(shared->printLock, 1, 1);
    sem_init(shared->boarded, 1, 0);
    shared->lines = 1;

    for(unsigned int i = 1; i < shared->stopAmount; i++) {
        shared->busStop[i]->waiting = 0;
        sem_init(shared->busStop[i]->boarding, 1, 0);
        sem_init(shared->busStop[i]->boarding, 1, 0);
    }
}

void destSem(shared_t *shared) {
    sem_destroy(shared->printLock);
     for(unsigned i = 1; i < shared->stopAmount; i++) {
        sem_destroy(shared->busStop[i]->boarding);
    }
    cleanMem(shared);
}

void printFile(shared_t *shared, char *object, ...) {
    sem_wait(shared->printLock);
    va_list args;
    va_start(args, object);

    fprintf(shared->file, "%ld: ", shared->lines++);
    vfprintf(shared->file, object, args);

    fflush(shared->file);
    va_end(args);
    sem_post(shared->printLock);
}

void skier(shared_t *shared, int id, int TL) {
    printFile(shared, "L %d: started\n", id);
    int randomTime = rand() % (TL);
    usleep(randomTime);
    int randomStop = rand() % (shared->stopAmount);
    shared->busStop[randomStop]->waiting++;

    

}

int main(int argc, char *argv[]) {

    parseArgs(argc, argv);

    int L = strtol(argv[1], NULL, 10);
    int Z = strtol(argv[2], NULL, 10);
    int K = strtol(argv[3], NULL, 10);
    int TL = strtol(argv[4], NULL, 10);
    int TB = strtol(argv[5], NULL, 10);

    if(!inRange(0, 19999, L) || !inRange(1, 10, Z) || !inRange(10, 100, K) || !inRange(0, 10000, TL) || !inRange(0, 1000, TB)) {
        errorMessage(argRange);
    }

    srand(time(NULL) ^ (getpid() << 16));

    shared_t *sharedMem;

    MMAP(sharedMem, sizeof(shared_t));
    MMAP(sharedMem->busStop, Z * sizeof(stop_t *));

    sharedMem->skierAmount = L;
    sharedMem->stopAmount = Z;
    sharedMem->busCapacity = K;

    initSem(sharedMem);

    sharedMem->file = fopen("proj2.out", "w");
    if(sharedMem->file == NULL) {
        errorMessage(fileError);
    }

    for(unsigned int i = 0; i < sharedMem->skierAmount; i++) {
        pid_t pid = fork();

        if(pid == 0) {
            skier(sharedMem, i, TL);
            exit(EXIT_SUCCESS);
        } else if(pid < 0) {
            errorMessage(memError);
        }
    }

    fclose(sharedMem->file);
    destSem(sharedMem);
    UNMAP(sharedMem, sizeof(shared_t));
    UNMAP(sharedMem->busStop, Z * sizeof(stop_t *));
    exit(EXIT_SUCCESS);
}