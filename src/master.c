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

#define BUFF_SIZE 1024

void intHandler(int);

int main(int argc, char const *argv[])
{
	key_t key;
	int shmid, i;
	pid_t childpid = 0;
	int status;
	shared_palinfo *shpinfo;

	char all_strings[BUFF_SIZE][BUFF_SIZE];
	int num_strings = 0, pos = 0;

	FILE* fp;
	char buf[BUFF_SIZE];

	if((fp = fopen("input.txt", "r")) == NULL){
		perror("Couldn't open input file.\n");
		return -1;
	}

	while (fgets(buf, sizeof(buf), fp) != NULL) {
		buf[strlen(buf) - 1] = '\0'; // eat the newline fgets() stores
		strcpy(all_strings[pos++], buf);
		num_strings++;
	}

	fclose(fp);

	// handle SIGNALS
	signal(SIGALRM, intHandler);
	signal(SIGINT, intHandler);

	// key = ftok(".", 'c');
	key = SHM_KEY;
	if(key == (key_t)-1) {
		fprintf(stderr, "Failed to derive key\n");
	}

	shmid = shmget(key, sizeof(shared_palinfo), IPC_CREAT | 0777);
	if((shmid == -1) && (errno != EEXIST))
		return -1;
	if(shmid == -1) {
		if (((shmid = shmget(key, sizeof(shared_palinfo), PERM)) == -1) || 
			(shpinfo = (shared_palinfo*)shmat(shmid, NULL, 0) == (void *)-1) )
			return -1;
	} else {
		shpinfo = shmat(shmid, NULL, 0);
		if(shpinfo == (void *)-1)
			return -1;

		shmid = shmget(key, num_strings*sizeof(char), IPC_CREAT | 0777);
		
		shpinfo -> status = 5;
		shpinfo -> proc_turn = 9;

		for (i = 0; i < num_strings; ++i)
 		{
 			strcpy( shpinfo->mylist[i] , all_strings[i] );
 		}

	}

	for (i = 0; i < 1; ++i)
		if ((childpid = fork()) <= 0)
			break;
	/* child process */
	if(childpid == 0) {
		

		// sprintf();


		char *palinOptions[] = {"./palin","1", "2", (char *)0};
		execv("./palin", palinOptions);
	} else { /* parent process */
		// fprintf(stderr, "Parent i:%d  process ID:%ld  parent ID:%ld  child ID:%ld\n",
  //          i, (long)getpid(), (long)getppid(), (long)childpid);
		
	}
    
    for(i = 0; i < 1; i++) {
	    childpid = wait(&status);
	    fprintf(stderr, "Master: Child %d has died....\n", childpid);
	}

	int error = 0;
	if(shmdt(shpinfo) == -1) {
		error = errno;
	}
	if((shmctl(shmid, IPC_RMID, NULL) == -1) && !error) {
		error = errno;
	}
	if(!error) {
		return 0;
	}

	return 0;
}

// handle interrupts

void intHandler(int SIGVAL) {
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);

	if(SIGVAL == SIGINT) {
		fprintf(stderr, "%sCTRL-C Interrupt\n");
	}

	if(SIGVAL == SIGALRM) {
		fprintf(stderr, "%sMaster timed out. Terminating rest all process.\n");
	}

	kill(-getpgrp(), SIGQUIT);

}
