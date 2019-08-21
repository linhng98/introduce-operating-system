#include "readers-writers.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t gMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gReaderCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t gWriterCond = PTHREAD_COND_INITIALIZER;

int main(int argc, char *argv[])
{
    int arrReaderNum[NUMBER_READERS];
    int arrWriterNum[NUMBER_WRITERS];

    pthread_t readers[NUMBER_READERS];
    pthread_t writers[NUMBER_WRITERS];

    srand(time(NULL));

    // start all readers
    for (int i = 0; i < NUMBER_READERS; i++)
    {
        arrReaderNum[i] = i + 1;
        pthread_create(&readers[i], NULL, readersMain, &arrReaderNum[i]);
    }

    // start all writers
    for (int i = 0; i < NUMBER_WRITERS; i++)
    {
        arrWriterNum[i] = i + 1;
        pthread_create(&writers[i], NULL, writersMain, &arrWriterNum[i]);
    }

    // wait readers to finish
    for (int i = 0; i < NUMBER_READERS; i++)
        pthread_join(readers[i], NULL);

    // wait writers to finish
    for (int i = 0; i < NUMBER_WRITERS; i++)
        pthread_join(writers[i], NULL);

    return 0;
}

void *readersMain(void *params)
{
    int thread_index = *(int *)params;

    for (int i = 0; i < READ_TIME; i++)
    {
        // wait so readers and writers not complete at once
        usleep(1000 * (rand() % NUMBER_READERS + NUMBER_WRITERS));

        // enter critical section
        pthread_mutex_lock(&gMutex);
        numWaitingReaders++;
        while (numReadingReaders == -1)
        {
            pthread_cond_wait(&gReaderCond, &gMutex);
        }
        numWaitingReaders--;
        numReadingReaders++;
        pthread_mutex_unlock(&gMutex);

        // read data
        printf("reader[%d] is reading : %d\n", thread_index, gSharedValue);

        // exit critical section
        pthread_mutex_lock(&gMutex);
        numReadingReaders--;
        if (numReadingReaders == 0)
            pthread_cond_signal(&gWriterCond);
        pthread_mutex_unlock(&gMutex);
    }
    return NULL;
}

void *writersMain(void *params)
{
    int thread_index = *(int *)params;

    for (int i = 0; i < WRITE_TIME; i++)
    {
        // wait so readers and writers not complete at once
        usleep(1000 * (rand() % NUMBER_READERS + NUMBER_WRITERS));

        // enter critical section
        pthread_mutex_lock(&gMutex);
        while (numReadingReaders != 0)
        {
            pthread_cond_wait(&gWriterCond, &gMutex);
        }
        numReadingReaders = -1;
        pthread_mutex_unlock(&gMutex);

        // write data
        gSharedValue++;
        printf("writer[%d] has writed : %d++\n", thread_index,
               gSharedValue - 1);

        // exit crital section
        pthread_mutex_lock(&gMutex);
        numReadingReaders = 0;
        if (numWaitingReaders > 0)
        {
            pthread_cond_broadcast(&gReaderCond);
        }
        else
        {
            pthread_cond_signal(&gWriterCond);
        }
        pthread_mutex_unlock(&gMutex);
    }
    return NULL;
}