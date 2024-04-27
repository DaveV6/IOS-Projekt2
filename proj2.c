/**************************************************************************************************
*   Project: IOS Project 2 - Semaphores
*
*   File:           proj2.c
*   Description:    The program simulates a bus which takes skiers from multiple bus stops to a final stop.
*
*   Date:           25.04.2024
*   Author:         David Bujzaš (xbujzad00)
**************************************************************************************************/

#include "proj2.h"

/// @brief checks argument count and parses all the arguments
/// @param argc argument count
/// @param argv array of input arguments
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

/// @brief switches between different types of errors
/// @param errType type of error
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

/// @brief checks if the value is between the minimum and maximum
/// @param min minimum value allowed
/// @param max maximum value allowed
/// @param value value which will be checked 
/// @return returns true or false depending on the check
bool inRange(int min, int max, int value) {
    return (value >= min && value <= max);
}

/// @brief initializes all the semaphores from the structure
/// @param shared structure for which semaphores are initialized
void initSem(shared_t *shared) {
    sem_init(&(shared->printLock), 1, 1);
    sem_init(&(shared->waitFinal), 1, 0);
    sem_init(&(shared->wentSkiing), 1, 0);
    shared->lines = 1;
    shared->onBoard = 0;
    shared->skiersSkiing = 0;

    for(unsigned int i = 0; i < shared->stopAmount; i++) {
        shared->busStop[i].skiersWaiting = 0;
        sem_init(&(shared->busStop[i].mutex), 1, 1);
        sem_init(&(shared->busStop[i].boarded), 1, 0);
        sem_init(&(shared->busStop[i].waiting), 1, 0);
    }
}

/// @brief destroys all initialized semaphores
/// @param shared structure for which semaphores are destroyed
void destSem(shared_t *shared) {
    sem_destroy(&(shared->printLock));
    sem_destroy(&(shared->waitFinal));
    sem_destroy(&(shared->wentSkiing));

     for(unsigned int i = 0; i < shared->stopAmount; i++) {
        sem_destroy(&(shared->busStop[i].mutex));
        sem_destroy(&(shared->busStop[i].boarded));
        sem_destroy(&(shared->busStop[i].waiting));
    }
}

/// @brief prints a sentence depending on the input arguments
/// @param shared structure
/// @param object format string for the message to be printed
/// @param ... variable number of additional arguments
void printFile(shared_t *shared, char *object, ...) {
    sem_wait(&(shared->printLock)); // locks the print semaphore in order to ensure everything is printed one by one
    // initialize variable argument list
    va_list args;
    va_start(args, object);
    // print the formated message to the file
    fprintf(shared->file, "%ld: ", shared->lines++); // prints the number of the row
    vfprintf(shared->file, object, args); // format and print additional arguments
    fflush(shared->file); // flushes the file buffer to ensure message is written immediately
    va_end(args); // end variable arguments list
    sem_post(&(shared->printLock)); // unlocks the semaphoe after one sentence is printed
}

/// @brief the bus process
/// @param shared structure
/// @param TB time between bus stops
void bus(shared_t *shared, int TB) {
    printFile(shared, "BUS: started\n"); // starts the bus

    // keeps going on until all skiers are skiing
    while(shared->skierAmount != shared->skiersSkiing) {
        // loops through all bus stops
        for(unsigned int id = 0; id < shared->stopAmount; id++) {
            int randTime = rand() % (TB) + 1; // random time
            usleep(randTime); // sets the bus to sleep for a random time in microseconds
            printFile(shared, "BUS: arrived to %d\n", id + 1); // sends the bus to the next stop
            sem_wait(&(shared->busStop[id].mutex)); // sets the mutex to wait in while decreasing the amount of skiers waiting
            // loops through all the skiers which are waiting at the stop
            for(unsigned int i = 0; i < shared->busStop[id].skiersWaiting; i++) {
                // if the bus is not full, skiers can board
                if(shared->busCapacity != shared->onBoard) {
                    sem_post(&(shared->busStop[id].waiting)); // waiting semaphore is unlocked and the skier can board
                    shared->busStop[id].skiersWaiting--; // after the skier boards, the amount of skiers waiting is decreased by 1
                    sem_wait(&(shared->busStop[id].boarded)); // waits until the skier is boarded
                }
            }
            sem_post(&(shared->busStop[id].mutex)); // unlocks the mutex semaphore
            printFile(shared, "BUS: leaving %d\n", id + 1); // the bus leaves the bus stop
        }
        printFile(shared, "BUS: arrived to final\n"); // after looping through x stops it arrives to the final stop
        // loops through all skiers aboard the bus
        for(unsigned int i = 0; i < shared->onBoard; i++) {
            sem_post(&(shared->waitFinal)); // unlocks the semaphore which waits for the final stop
            sem_wait(&(shared->wentSkiing)); // waits for the skier to leave the bus and go skiing
        }
        shared->onBoard = 0; // sets the number of skiers aboard the bus to 0, after they go skiing
        printFile(shared, "BUS: leaving final\n"); // bus leaves the final stop and restarts the loop until all skiers are accounted for
    }
    printFile(shared, "BUS: finish\n"); // the bus process is over

}

/// @brief the skier process
/// @param shared structure
/// @param id skier id
/// @param TL time between skier action
void skier(shared_t *shared, int id, int TL) {
    printFile(shared, "L %d: started\n", id); // starts the skiers
    int randTime = rand() % (TL) + 1; // random time
    usleep(randTime); // sets the skiers to sleep for a random time in microseconds
    int randomStop = rand() % (shared->stopAmount); // generates a random stop from all the stops

    sem_wait(&(shared->busStop[randomStop].mutex)); // sets mutex semaphor to wait, ensures all skiers are accounted for
    shared->busStop[randomStop].skiersWaiting++; // increments the amount of skiers waiting at a stop
    sem_post(&(shared->busStop[randomStop].mutex)); // unlocks the mutex semaphor after a skier starts waiting
    printFile(shared, "L %d: arrived to %d\n", id, randomStop + 1); // sends the skiers to a random stop
    
    sem_wait(&(shared->busStop[randomStop].waiting)); // ensures the skiers wait for the bus to arrive in order to board
    shared->onBoard++; // after the bus arrives we increase the amount of skiers on board
    printFile(shared, "L %d: boarding\n", id); // skiers board the bus
    sem_post(&(shared->busStop[randomStop].boarded)); // signals that all skiers at the stop were boarded

    sem_wait(&(shared->waitFinal)); // waiting for the final bus stop
    shared->skiersSkiing++; // increments the amount of skiers going to ski
    printFile(shared, "L %d: going to ski\n", id); // after arriving at the final stop, all skiers aboard leave to ski
    sem_post(&(shared->wentSkiing)); // signals that the skiers left the bus
}

int main(int argc, char *argv[]) {

    parseArgs(argc, argv);

    int L = strtol(argv[1], NULL, 10);
    int Z = strtol(argv[2], NULL, 10);
    int K = strtol(argv[3], NULL, 10);
    int TL = strtol(argv[4], NULL, 10);
    int TB = strtol(argv[5], NULL, 10);

    // checks if all the arguments are in the allowed range
    if(!inRange(0, 19999, L) || !inRange(1, 10, Z) || !inRange(10, 100, K) || !inRange(0, 10000, TL) || !inRange(0, 1000, TB)) {
        errorMessage(argRange);
    }

    // sets random seed
    srand(time(NULL) ^ (getpid() << 16));

    shared_t *sharedMem;

    // mapping the used structures
    MMAP(sharedMem, sizeof(shared_t));
    MMAP(sharedMem->busStop, Z * sizeof(stop_t));

    sharedMem->skierAmount = L;
    sharedMem->stopAmount = Z;
    sharedMem->busCapacity = K;

    initSem(sharedMem);

    // opens the file and writes in it
    sharedMem->file = fopen("proj2.out", "w");
    if(sharedMem->file == NULL) {
        errorMessage(fileError);
    }

    // forks the bus process
    pid_t busPid = fork();

    // if 0 the bus process starts
    if(busPid == 0) {
        bus(sharedMem, TB);
        exit(EXIT_SUCCESS);
    // if busPid is lower than 0 exits with FAILURE, destroys semaphores and unmaps the structures
    } else if(busPid < 0) {
        destSem(sharedMem);
        UNMAP(sharedMem->busStop, Z * sizeof(stop_t));
        UNMAP(sharedMem, sizeof(shared_t));
        errorMessage(memError);
    }

    // loops through the skier amount and forks them
    for(unsigned int i = 1; i <= sharedMem->skierAmount; i++) {
        pid_t pid = fork();

        // if 0 the skier process starts
        if(pid == 0) {
            skier(sharedMem, i, TL);
            exit(EXIT_SUCCESS);
        // if pid is lower than 0 exits with FAILURE, destroys semaphores and unmaps the structures
        } else if(pid < 0) {
            destSem(sharedMem);
            UNMAP(sharedMem->busStop, Z * sizeof(stop_t));
            UNMAP(sharedMem, sizeof(shared_t));
            errorMessage(memError);
        }
    }

    // waiting for all the children to die
    while(wait(NULL) > 0);

    // if all is successful, the semaphores get destroyed, structures unmapped and the program exits with SUCCESS
    destSem(sharedMem);
    UNMAP(sharedMem->busStop, Z * sizeof(stop_t));
    UNMAP(sharedMem, sizeof(shared_t));
    exit(EXIT_SUCCESS);
}

/*********************************END OF FILE PROJ2.C*********************************/