#ifndef _SHM_HEADER_H
#define _SM_HEADER_H

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define SHM_KEY 19942017
#define MAX_BUF_SIZE 100

enum pState { idle, want_in, in_cs }; 

typedef struct 
{
	int pturn;
	int flag[20];
	char *buffer[];
} shared_palinfo;

#endif