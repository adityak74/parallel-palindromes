#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "shm_header.h"

#define PERM 0666

int procNum = 0;
pid_t myPid;

int main(int argc, char const *argv[])
{
	key_t key = SHM_KEY;
	int shmid, i, stringIndex;
	shared_palinfo *shpalinfo;
	char *palin_out_filename = "palin.out";
	char *nopalin_out_filename = "nopalin.out";
	myPid = getpid();
	int max_writes = 5;
	FILE *palin_fp, *nopalin_fp;
	char *short_options = "i:n:x:h::";
	char c;

	//get options from parent process
	opterr = 0;
	while((c = getopt(argc, argv, short_options)) != -1) 
	switch (c) {
	  case 'i':
	    max_writes = atoi(optarg);
	    break;
	  case 'n':
	    procNum = atoi(optarg);
	    break;
	  case 'x':
	  	stringIndex = atoi(optarg);
	  case '?':
	    fprintf(stderr, "    Arguments were not passed correctly to slave %d. Terminating.", myPid);
	    exit(-1);
	}

	shmid = shmget(key, sizeof(shared_palinfo), PERM);
	
	if(shmid == -1) {
		if (((shmid = shmget(key, sizeof(shared_palinfo), PERM)) == -1) || 
			(shpalinfo = (shared_palinfo*)shmat(shmid, NULL, 0) == (void *)-1) )
			return -1;
	} else {
		shpalinfo = (shared_palinfo*)shmat(shmid, NULL, 0);
		if(shpalinfo == (void *)-1)
			return -1;

		if(shmdt(shpalinfo) == -1) {
		    perror("Slave could not detach shared memory");
		}

		
	}

	return 0;
}