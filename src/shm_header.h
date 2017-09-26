#ifndef _SHM_HEADER_H
#define _SHM_HEADER_H

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

enum pState { idle, want_in, in_cs }; 

typedef struct 
{
	int turn;
	int flag[20];
	char buffer[MAX_BUF_SIZE][MAX_BUF_SIZE];
} shared_palinfo;

#endif