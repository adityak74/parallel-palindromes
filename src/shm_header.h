#ifndef _SHM_HEADER_H
#define _SHM_HEADER_H

#define SHM_KEY 19942017
#define MAX_BUF_SIZE 100

enum pState { idle, want_in, in_cs }; 

typedef struct 
{
	int proc_turn;
	int totalProcesses;
	enum pState flag[20];
	char mylist[MAX_BUF_SIZE][MAX_BUF_SIZE];
} shared_palinfo;

#endif