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
#define MAXCHILD 20

void intHandler(int);
void showHelpMessage();
static int max_processes_at_instant = 0; // to count the max running processes at any instance

int main(int argc, char const *argv[])
{
	key_t key;
	int shmid, i;
	pid_t childpid = 0;
	int status;
	shared_palinfo *shpinfo;
	int timerVal;
	int numChildren;
	int max_writes, helpflag = 0, maxwriteValue, index, nonoptargflag;
	char *short_options = "hc:w:t:";

	char all_strings[MAX_BUF_SIZE][MAX_BUF_SIZE];
	int num_strings = 0, pos = 0;
	char c;
	FILE* fp;
	char buf[MAX_BUF_SIZE];

	opterr = 0;
	while ((c = getopt (argc, argv, short_options)) != -1)
	switch (c) {
	case 'h':
		helpflag = 1;
		break;
	case 'c':
		numChildren = atoi(optarg);
		if(numChildren > MAXCHILD) {
		  numChildren = 19;
		  fprintf(stderr, "No more than 19 child processes allowed. Reverting to 19.\n");
		}
		break;
	case 'w':
		max_writes = atoi(optarg);
		break;
	case 't':
		timerVal = atoi(optarg);  
		break;
	case '?':
		if (optopt == 's') {
		  fprintf(stderr, "Option -%c requires an argument. Using default value [19].\n", optopt);
		  numChildren = 19;
		}
		else if (optopt == 'w') {
		  fprintf(stderr, "Option -%c requires an argument. Using default value [5].\n", optopt);
		  max_writes = 5;
		}
		else if (optopt == 't') {
		  fprintf(stderr, "Option -%c requires an argument. Using default value [60].\n", optopt);
		  timerVal = 60;
		}
		else if (isprint (optopt)) {
		  fprintf(stderr, "Unknown option -%c. Terminating.\n", optopt);
		  return -1;
		}
		else {
		  showHelpMessage();
		  return 0; 
		}
	}

	//print out all non-option arguments
	for (index = optind; index < argc; index++) {
		fprintf(stderr, "Non-option argument %s\n", argv[index]);
		nonoptargflag = 1;
	}

	//if above printed out, print help message
	//and return from process
	if(nonoptargflag) {
		showHelpMessage();
		return 0;
	}

	//if help flag was activated, print help message
	//then return from process
	if(helpflag) {
		showHelpMessage();
		return 0;
	}

	if( numChildren<=0 || max_writes<=0 || timerVal<=0 ) {
		showHelpMessage();
		return 0;
	}

	//** Read strings to check from file **//
	
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
	//** END **//

	// handle SIGNALS callback attached
	signal(SIGALRM, intHandler);
	signal(SIGINT, intHandler);

	//set alarm
	alarm(timerVal);

	// generate key using ftok
	key = ftok(".", 'c');

	// key = SHM_KEY;
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
	char *k_arg = malloc(sizeof(char)*20); 
	
	fprintf(stderr, "Total strings to process: %d\n", num_strings);

	for (i = 0; i < numChildren; ++i){
		max_processes_at_instant++;
		if ((childpid = fork()) <= 0)
			break;
	}

	/* child process */
	if(childpid == 0) {

		fprintf(stderr, "Max processes running now: %d\n", max_processes_at_instant);

		childpid = getpid();
		sprintf(i_arg, "%d", i);
		sprintf(m_arg, "%d", max_writes);
		// xx string id to test for palin for child
		sprintf(x_arg, "%d", i*max_writes);
		// max string ID to break
		sprintf(s_arg, "%d", num_strings-1);
		// share shmid with children
		sprintf(k_arg, "%d", shmid);
		char *palinOptions[] = {"./palin", "-i", i_arg, "-m", m_arg, "-x", x_arg, "-s", s_arg, "-k", k_arg, (char *)0};
		execv("./palin", palinOptions);
	} else { /* parent process */
		
	}
	
	// clear up args
	free(i_arg);
	free(m_arg);
	free(x_arg);
	free(s_arg);
	free(k_arg);
    
    for(i = 0; i < numChildren; i++) {
	    childpid = wait(&status);
	    if(childpid != -1) { // skip for a failed fork
	    	max_processes_at_instant--;
	    	fprintf(stderr, "Max processes running now: %d\n", max_processes_at_instant);
	    	fprintf(stderr, "Master: Child %d has died...\n", childpid);
	    }
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
		fprintf(stderr, "%sCTRL-C Interrupt initiated.\n");
	}

	if(SIGVAL == SIGALRM) {
		fprintf(stderr, "%sMaster timed out. Terminating rest all process.\n");
	}

	kill(-getpgrp(), SIGQUIT);

}

// help message for running options
void showHelpMessage() {
	printf("-h: Prints this help message.\n");
    printf("-c: Allows you to set the number of child process to run.\n");
    printf("\tThe default value is 19. The max is 19.\n");
    printf("-w: Allows you to set the number of times each child enters the critical section of code(writes to file).\n");
    printf("\tThe default value is 5.\n");
    printf("-t: Allows you set the wait time for the master process until it kills the slaves.\n");
    printf("\tThe default value is 60.\n");
}
