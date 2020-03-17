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
int count = 0;
int status = 0;
int launched = 0;
int *pidsArr;
int pids = 0;
//=========================================================================
//Memory sharing stuff
//=========================================================================

//shared memory array holding ints
key_t key1 = ARRAY;
int arrId = 0;
int *arrPtr = NULL;
int *sharedArray(key_t, size_t, int*);

key_t key2 = SEMP;
int semId = 0;
sem_t *semPtr = NULL;
size_t semSize = sizeof(sem_t);
sem_t *sharedSem(key_t , size_t, int *);


//detach and remove memory
void cleanUp(void *, int);

//=======================================================================
//helper functions
//======================================================================

void fillArray(int);
int square(int);
void forkNDivided(int);

//========================================================================
//child stuff
//========================================================================
pid_t childPid = 0;

int exitPid = 0;

void killAll(int);
void timesUp(int);
//========================================================================


int main(int argc, char *argv[])
{
	
	setopt(argc, argv);

	if(arg % 2 != 0)
	{
		printf("number must be a power of 2\n");
		exit(1);
	}
	
	
	if(helpFlag)
	{
		help();
	}
	
	signal(SIGINT, killAll);
	signal(SIGALRM, timesUp);
	alarm(100);

	writeTo();

	arrPtr =  sharedArray(key1, count * sizeof(int), &arrId);
	semPtr = sharedSem(key2, semSize, &semId);

	fillArray(arg);

	FILE *fp;
	time_t start;
	time_t end;
	float result;

	fp = fopen("adder_log.dat", "a");
	if(fp == NULL)
	{
		perror("ERROR: master: fp(main)\n");
		exit(1);
	}

	fprintf(fp, "\nLog of %d/2 process\n", arg);
	fprintf(fp, "\n\t-------------------\n");
	fprintf(fp, "\t\tPID\t\tINDEX\t\tSIZE\n\n");

	time(&start);	
	forkNDivided(arg);
	time(&end);

	result = end - start;

	fprintf(fp,"\n\n");
	fprintf(fp,"time for %d/2 processess is time = %.2f\n\n", arg, result);
	
	fclose(fp);

	cleanUp(arrPtr, arrId);
	cleanUp(semPtr, semId);
	
	return 0;
}



void forkNDivided(int arg)
{

	int position;
	int i;
	int nums = arg;
	int squared = square(arg);

	pidsArr = (int *)calloc(arg, sizeof(int));
	
	for(i = 1; i <= squared; i++)
	{	
		position = 0;
		while(1)
		{
			if(launched < 20)
			{
				childPid = fork();

				if(childPid < 0)
				{
					perror("ERROR: master: fork\n");
					exit(1);
				}
				
				if(childPid == 0)
				{
					char xx[20];
					char yy[20];
					snprintf(xx, sizeof(xx), "%d", position);
					snprintf(yy, sizeof(yy), "%d", nums);
					execlp("./bin_adder", xx, yy, NULL);
				}

				pidsArr[pids] = childPid;
				pids++;
				position++;
				launched++;
				
			}

			if((exitPid = waitpid((pid_t)-1, &status, 0)) > 0)
			{
				if(WIFEXITED(status))
				{
					launched--;
				}
			}
			

			if(position >= nums)
				break;
		}


		nums = nums / 2;
		
		if(nums == 1)
		{
			break;
		}
	}
		
	
}

//==============================================================================
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

void fillArray(int size)
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
	


	for(i = 0; i < size; i++)
	{
		arrPtr[i] = atoi(str[i]);
	}	
	
	
	
	fclose(fp);
}
 
int square(int num)
{
	if(num == 0 || num == 1)
		return num;
	
	int i = 1, result = 1;
	while(result <= num)
	{ 
		i++;
		result = i * i;
	}
	return i - 1;
}

//===========================================================================

int *sharedArray(key_t key, size_t size, int *shmid)
{
	*shmid = shmget(key, size, PERM);
	if(*shmid < 0)
	{
		perror("ERROR: master: shmget(sharedArray)\n");
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

sem_t *sharedSem(key_t key, size_t size, int *shmid)
{
	*shmid = shmget(key, size, PERM);
	if(*shmid < 0)
	{
		perror("ERROR: master: shmget(sharedSem)\n");
		exit(1);
	}

	sem_t *temp = (sem_t *)shmat(*shmid, NULL, 0);
	if(temp == (sem_t *)-1)
	{
		perror("ERROR: master: shmat(sharedSem)\n");
		exit(1);
	}

	if(sem_init(temp, 1, 1) == -1)
	{
		perror("ERROR: master: sem_init(sharedSem)\n");
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


void killAll(int sig)
{
	char msg[] = "\nkilling all processes\n";
	int msgSize = sizeof(msg);
	write(STDERR_FILENO, msg, msgSize);

	shmctl(arrId, IPC_RMID, NULL);
	shmctl(semId, IPC_RMID, NULL);

	int i;
	for(i = 0; i < pids; i++)
	{
		if(pidsArr[i] !=  0)
		{
			if(kill(pidsArr[i], SIGTERM) == -1)
			{
				perror("ERROR: master: SIGTERM(killAll)\n");
			}
		}
	}
	
	free(pidsArr);
	exit(0);
}

void timesUp(int sig)
{
	char msg[] = "\nprogram has reached 100 seconds\n";
	int msgSize = sizeof(msg);
	write(STDERR_FILENO, msg, msgSize);

	shmctl(arrId, IPC_RMID, NULL);
	shmctl(semId, IPC_RMID, NULL);
	int i;
	for(i = 0; i < pids; i++)
	{
		if(pidsArr[i] != 0)
		{
			if(kill(pidsArr[i], SIGTERM) == -1)
			{
				perror("ERROR: master: SIGTERM(timesUp)\n");
			}
		}
	}

	free(pidsArr);
	exit(0);
}

//============================================================================

void help()
{
	printf("-----HELP MESSAGE-----\n\n");
	printf("-r: number of integers to add between 16 and 64. default is 16\n");
}

void print()
{
	printf("-h = %d\n",helpFlag);
	printf("-r = %d\n", arg);
}
