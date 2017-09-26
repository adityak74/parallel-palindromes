#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

typedef struct shared_palinfo
{
	enum state { idle, want_in, in_cs };
	int turn;
	int flag[20];
};