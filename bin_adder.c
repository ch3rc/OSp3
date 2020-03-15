//======================================================================
//Date:		March 12,2020
//Author:	Cody Hawkins
//Class:	CS4760
//Project:	Assignment 3
//File:		bin_adder.c
//======================================================================

#include "keys.h"

#define PERM (IPC_CREAT | 0666)

int count;

//======================================================================
//shared memory stuff
//======================================================================

key_t key1 = ARRAY;
int arrId = 0;
int *arrPtr = NULL;
int *getArray(key_t, size_t, int *);

//======================================================================


int main(int argc, char *argv[])
{
	arrPtr = getArray(key1, count * sizeof(int), &arrId);

	printf("child has recieved memory array\n");
	printf("count = %d\n", count);
	int i;
	for(i = 0; i < 64; i++)
	{
		printf("%d\n", arrPtr[i]);
	}

	printf("child detaching memory\n");

	if(shmdt(arrPtr) == -1)
	{
		perror("ERROR: bin_adder: shmdt\n");
		exit(1);
	}
	
	printf("child finished\n");
	
	return 0;
}

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

