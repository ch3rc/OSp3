//==============================================================
//Date:		March 12, 2020
//Author:	Cody Hawkins
//Class:	CS4760
//Project:	Assignment 3
//File:		keys.h
//=============================================================

#ifndef KEYS_H
#define KEYS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#define ARRAY 0x12345678

extern int helpFlag;
extern int arg;
extern char logFile[];
extern char adder[];
extern int count;

void setopt(int, char **);
void help();
void print();
void writeTo();


#endif
