#ifndef READERS_WRITERS_H_
#define READERS_WRITERS_H_

#define NUMBER_READERS 3
#define NUMBER_WRITERS 3
#define READ_TIME 2
#define WRITE_TIME 2

int gSharedValue = 0;
int numReadingReaders = 0;
int numWaitingReaders = 0;

void *readersMain(void *param);
void *writersMain(void *param);

#endif