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
#include <time.h>

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
	int timerVal = 60;
	int numChildren = 19;
	int max_writes = 5;

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

	//set alarm
	alarm(timerVal);

	// key = ftok(".", 'c');
	key = SHM_KEY;
	if(key == (key_t)-1) {
		fprintf(stderr, "Failed to derive key\n");
	}

	shmid = shmget(key, sizeof(shared_palinfo), IPC_CREAT | 0777);
	if((shmid == -1) && (errno != EEXIST)){
		perror("Unable to create shared mem");
		exit(-1);
	}
	if(shmid == -1) {
		if (((shmid = shmget(key, sizeof(shared_palinfo), PERM)) == -1) || 
			(shpinfo = (shared_palinfo*)shmat(shmid, NULL, 0) == (void *)-1) ) {
			perror("Unable to attach existing shared memory");
			exit(-1);
		}
	} else {
		shpinfo = shmat(shmid, NULL, 0);
		if(shpinfo == (void *)-1){
			perror("Couldn't attach the shared mem");
			exit(-1);
		}

		// initialize shmem params
		shpinfo -> proc_turn = 0;
		shpinfo -> totalProcesses = numChildren;

		// set idle flags to all process
		for (i = 0; i < numChildren; ++i)
		{
			shpinfo -> flag[i] = idle;
		}

		// copy strings to shared memory
		for (i = 0; i < num_strings; ++i)
 		{
 			strcpy( shpinfo->mylist[i] , all_strings[i] );
 		}

	}

	char *i_arg = malloc(sizeof(char)*20);
	char *m_arg = malloc(sizeof(char)*20);
	char *x_arg = malloc(sizeof(char)*20);
	char *s_arg = malloc(sizeof(char)*20); 

	
	fprintf(stderr, "Total strings to process : %d\n", num_strings);

	for (i = 0; i < numChildren; ++i)
		if ((childpid = fork()) <= 0)
			break;

	/* child process */
	if(childpid == 0) {
		childpid = getpid();
		sprintf(i_arg, "%d", i);
		sprintf(m_arg, "%d", max_writes);
		
		// xx string id to test for palin for child
		sprintf(x_arg, "%d", i*max_writes);

		// max string ID to break
		sprintf(s_arg, "%d", num_strings-1);

		// fprintf(stderr, "In Child : %d :: %d %d %d %d\n", childpid, i, max_writes, i*max_writes, num_strings-1 );

		char *palinOptions[] = {"./palin", "-i", i_arg, "-m", m_arg, "-x", x_arg, "-s", s_arg, (char *)0};
		execv("./palin", palinOptions);
	} else { /* parent process */
		
	}
	
	// clear up args
	free(i_arg);
	free(m_arg);
	free(x_arg);
	free(s_arg);

    
    for(i = 0; i < numChildren; i++) {
	    childpid = wait(&status);
	    if(childpid != -1) // skip for a failed fork
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
