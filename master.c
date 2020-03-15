//=========================================================================
//Date:		March 12, 2020
//Author:	Cody Hawkins
//Class:	CS4760
//Project:	Assignment 3
//File:		Master.c
//=========================================================================

#include "keys.h"

#define PERM (IPC_CREAT | 0666)

//=========================================================================
//Global stuff
//=========================================================================

int helpFlag = 0;
int arg = 16;
char logFile[] = "log.dat";
char adder[] = "adder_log.dat";
int count;
int status = 0;

//=========================================================================
//Memory sharing stuff
//=========================================================================

//shared memory array holding ints
key_t key1 = ARRAY;
int arrId = 0;
int *arrPtr = NULL;
int *sharedArray(key_t, size_t, int*);

//detach and remove memory
void cleanUp(void *, int);

void fillArray();
//========================================================================
//child stuff
//========================================================================
pid_t childPid = 0;

int exitPid = 0;

//========================================================================


int main(int argc, char *argv[])
{
	count = 0;
	
	setopt(argc, argv);
	
	if(helpFlag)
	{
		help();
	}
	
	writeTo();

	arrPtr =  sharedArray(key1, count * sizeof(int), &arrId);

	fillArray();
	
	//test one child to make sure shared memory is set up properly
	childPid = fork();

	if(childPid < 0)
	{
		perror("ERROR: master: fork\n");
		exit(1);
	}

	if(childPid == 0)
	{
		char str[25];
		snprintf(str, sizeof(str), "%d", 1);
		execlp("./bin_adder",str, NULL);
	}
	
	if((exitPid = waitpid(childPid, &status, 0)) > 0)
	{
		if(WIFEXITED(status))
		{
			printf("child has finished!\n");
		}
	}

	printf("MASTER IS CLEANING UP MEMORY\n");
	cleanUp(arrPtr, arrId);
	printf("MASTER HAS FINISHED REMOVING MEMORY\n");

	print();

	return 0;
	
}

void setopt(int argc, char **argv)
{
	int c;

	while((c = getopt(argc, argv, "hr:")) != -1)
	{
		switch(c)
		{
			case 'h':
				helpFlag = 1;
				break;

			case 'r':
				arg = atoi(optarg);
				break;

			case '?':
				fprintf(stderr, "unknown option\n");
				exit(EXIT_FAILURE);
		}
	}
}

void writeTo()
{
	srand(time(0));
	int i;
	FILE *fp;
	fp = fopen(logFile, "w");
	for(i = 0; i < 64; i++)
	{
		int num = (rand() % (256 - 0 + 1)) + 0;
		fprintf(fp, "%d\n", num);
		count++;
	}
		
	fclose(fp);
}

void fillArray()
{
	char buf[1024];
	char *str[1024];
	FILE *fp;
	int j = 0;
	int i;
	fp = fopen(logFile, "r");
	
	while(fscanf(fp, "%s", buf) != EOF)
	{
		str[j] = malloc(sizeof(buf) + 1);
		strcpy(str[j], buf);
		j++;
	}
	


	for(i = 0; i < count; i++)
	{
		arrPtr[i] = atoi(str[i]);
	}
	
	
	
	fclose(fp);
}

//===========================================================================

int *sharedArray(key_t key, size_t size, int *shmid)
{
	*shmid = shmget(key, size, PERM);
	if(*shmid < 0)
	{
		perror("ERROR: master: shmat(sharedArray)\n");
		exit(1);
	}
	
	int *temp = (int *)shmat(*shmid, NULL, 0);
	if(temp == (void *)-1)
	{
		perror("ERROR: master: shmat (sharedArray)\n");
		exit(1);
	}

	return temp;
}

void cleanUp(void *ptr, int id)
{
	if(shmdt(ptr) == -1)
	{
		perror("ERROR: master: shmdt (cleanUp)\n");
		exit(1);
	}

	if(shmctl(id, IPC_RMID, NULL) == -1)
	{
		perror("ERROR: master: shmctl (cleanUp)\n");
		exit(1);
	}
}

//============================================================================

void help()
{
	printf("-----HELP MESSAGE-----\n\n");
	printf("-r: number to imput, between 16 and 128. default is 16\n");
}

void print()
{
	printf("-h = %d\n",helpFlag);
	printf("-r = %d\n", arg);
}
