//======================================================================
//Date:		March 12,2020
//Author:	Cody Hawkins
//Class:	CS4760
//Project:	Assignment 3
//File:		bin_adder.c
//======================================================================

#include "keys.h"

#define PERM (IPC_CREAT | 0666)
int debug;
int count;
char adder[] = "adder_log.dat";
//======================================================================
//shared memory stuff
//======================================================================

key_t key1 = ARRAY;
int arrId = 0;
int *arrPtr = NULL;
int *getArray(key_t, size_t, int *);

key_t key2 = SEMP;
int semId = 0;
size_t semSize = sizeof(sem_t);
sem_t *semPtr = NULL;
sem_t *getSharedSem(key_t, size_t, int *);


void removeMem(void *);

void nDivided(int, int);

void logDivided(int, int);
//======================================================================
//
//======================================================================


int main(int argc, char *argv[])
{
	arrPtr = getArray(key1, count * sizeof(int), &arrId);	
	semPtr = getSharedSem(key2, semSize, &semId);

	int start = atoi(argv[0]);
	int end = atoi(argv[1]);

	if(debug == 0)
	{
		nDivided(start, end);
	}


	if(debug == 1)
	{
		logDivided(start, end);
	}
	

	exit(12);
}


void logDivided(start, end)
{
	FILE *fp;
	fp = fopen(adder, "a");
	if(fp == NULL)
	{
		perror("ERROR: bin_adder: fp(logDivided\n");
		exit(1);
	}

	int child1;
	int child2;
	int sum;
	int half = end/2;
	int second = start + half;
	child1 = start;
	child2 = second;

	//same process used with method one. start a position zero for first child
	//start at start + half of number of processess for second child
	if(child1 < half)
	{
		//sum of each number
		sum = arrPtr[child1] + arrPtr[child2];

		sem_wait(semPtr);
	
		fprintf(stderr,"logn start = %d, end = %d\n\n", start, end);
		
		fprintf(stderr, "\nentering: child: %d time: %ld\n",getpid(), time(NULL));
	
		sleep(1);
		//critical section
		fprintf(fp, "\n\t%d\t%d\t%d",getpid(), start, end);
	
		fprintf(fp, "\t %d and %d \t %d + %d \t %d\t%d\n", child1, child2,
			arrPtr[child1], arrPtr[child2], child1, sum);
	
		sleep(1);

		fprintf(stderr, "\nexiting: child: %d time %ld\n", getpid(), time(NULL));

		sem_post(semPtr);
		//sum saved to first position for next round
		arrPtr[child1] = sum;
	}

	removeMem(arrPtr);
	removeMem(semPtr);
	//exit(12);
}


void nDivided(int start, int end)
{
	FILE *fp;
	fp = fopen(adder, "a");
	if(fp == NULL)
	{
		perror("ERROR: bin_adder: fp(nDivided)\n");
		exit(1);
	}
	
	int child1;
	int child2;
	int sum;
	int half = end / 2;
	int second = start + half;
	child1 = start;
	child2 = second;

	//start child1 at position zero and child2 at position 0 + half of end and increment
	//start until halfway, if child is one below half we stop counting for this pair of
	//processes.
	if(child1 < half)
	{
		sum = arrPtr[child1] + arrPtr[child2];
		
		sem_wait(semPtr);
		
		fprintf(stderr, "\n start = %d, end = %d\n",start, end);		

		fprintf(stderr, "\nentering:  child: %d time %ld\n", getpid(), time(NULL));

		sleep(1);
		//critical section
		fprintf(fp,"\n\t%d\t%d\t%d", getpid(), start, end);

		fprintf(fp, "\t%d and %d \t %d + %d \t %d\t%d\n", child1, child2,
			arrPtr[child1], arrPtr[child2], child1, sum);

		sleep(1);

		fprintf(stderr, "\nexiting:  child: %d time %ld\n", getpid(), time(NULL));

		sem_post(semPtr);
	
		arrPtr[child1] = sum;
	}
	fclose(fp);
	removeMem(arrPtr);
	removeMem(semPtr);
	//exit(12);
}

//=======================================================================
//shared memory functions
//=======================================================================
int *getArray(key_t key, size_t size, int *shmid)
{
	*shmid = shmget(key, size, PERM);
	if(*shmid < 0)
	{
		perror("ERROR: bin_adder: shmget(getArray)\n");
		exit(1);
	}

	int *temp = (int *)shmat(*shmid, NULL, 0);
	if(temp == (void *)-1)
	{
		perror("ERROR: bin_adder: shmat(getArray)\n");
		exit(1);
	}

	return temp;
}

sem_t *getSharedSem(key_t key, size_t size, int *shmid)
{
	*shmid = shmget(key, size, PERM);
	if(*shmid < 0)
	{
		perror("ERROR: bin_adder: shmget(getSharedSem)\n");
		exit(1);
	}
	
	sem_t *temp = (sem_t *)shmat(*shmid, 0, 0);
	if(temp == (sem_t *)-1)
	{
		perror("ERROR: bin_adder: shmat(getSharedSem)\n");
		exit(1);
	}

	return temp;
}

void removeMem(void *ptr)
{
	if(shmdt(ptr) == -1)
	{
		perror("ERROR: bin_adder: shmdt(remove)\n");
		exit(1);
	}
}
