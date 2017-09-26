#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "shm_info.h"
#define PERM (S_IRUSR | S_IWUSR)

int main(int argc, char const *argv[])
{
	key_t key;
	int shmid;

	struct shared_palinfo *shpinfo;

	shmid = shmget(key, sizeof(shared_palinfo), PERM);
	
	if(shmid == -1) {
		if (((shmid = shmget(key, sizeof(shared_palinfo), PERM)) == -1) || 
			(shpinfo = (shared_palinfo*)shmat(shmid, NULL, 0) == (void *)-1) )
			return -1;
	} else {
		shpinfo = shmat(shmid, NULL, 0);
		if(shpinfo == (void *)-1)
			return -1;
		fprintf(stderr, "%d\n", shpinfo -> turn);
		fprintf(stderr, "%d\n", shpinfo -> flag[0]);
	}

	return 0;
}