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
#include <signal.h>
#include <sys/mman.h>
#include <time.h>

#include "shm_header.h"

#define PERM 0666

int procNum = 0;
pid_t myPid;
int check_palindrome(char[]);
void signal_quit_handler(int);
void alarm_handler(int);
volatile sig_atomic_t signal_flag = 1;
const int KILL_TIMEOUT = 10;

int main(int argc, char const *argv[])
{
	srand(time(NULL));
	key_t key = SHM_KEY;
	int shmid, i, stringIndex;
	shared_palinfo *shpalinfo;
	char *palin_out_filename = "palin.out";
	char *nopalin_out_filename = "nopalin.out";
	myPid = getpid();
	int max_writes_in = 0;
	FILE *file;
	char *short_options = "i:m:x:ms:h::";
	int max_string_index = 0;
	char c;

	//get options from parent process
	opterr = 0;
	while((c = getopt(argc, argv, short_options)) != -1) 
	switch (c) {
	  case 'i':
	    procNum = atoi(optarg);
	    break;
	  case 'm':
	    max_writes_in = atoi(optarg);
	    break;
	  case 'x':
	  	stringIndex = atoi(optarg);
	  	break;
	  case 's':
	  	max_string_index = atoi(optarg);
	  	break;
	  case '?':
	    fprintf(stderr, "    Arguments were not passed correctly to slave %d. Terminating.", myPid);
	    exit(-1);
	}
	
	// fprintf(stderr, "In Child : %d :: %d %d %d %d\n",myPid, procNum, max_writes_in, stringIndex, max_string_index );
	// exit(-1);
	shmid = shmget(key, sizeof(shared_palinfo), PERM);
	
	if(shmid == -1) {
		if (((shmid = shmget(key, sizeof(shared_palinfo), PERM)) == -1) || 
			(shpalinfo = (shared_palinfo*)shmat(shmid, NULL, 0) == (void *)-1) ) {
			perror("Unable to access shared mem");
			exit(-1);
		}
	} else {
		shpalinfo = (shared_palinfo*)shmat(shmid, NULL, 0);
		if(shpalinfo == (void *)-1){
			perror("Couldn't attach the shared mem");
			exit(-1);
		}
		
	}

	//Ignore SIGINT so that it can be handled
	signal(SIGINT, SIG_IGN);

	//Set the sigquitHandler for the SIGQUIT signal
	signal(SIGQUIT, signal_quit_handler);

	//Set the alarm handler
	signal(SIGALRM, alarm_handler);

	//Set an alarm to 5-10 more seconds than the parent process
	//so that the child will be killed if parents gets killed
	alarm(65);

	i = 0;
	int j;
	int random;

	while(i < max_writes_in && signal_flag && (stringIndex+i)<=max_string_index) {
	    do {

	      //Raise my flag
	      shpalinfo->flag[procNum] = want_in;
	      //Set local variable
	      j = shpalinfo->proc_turn;

	      //Wait until it's my proc_turn
	      //This while loop short circuits on the j process if it is not idle, otherwise it goes to then next process then 
	      //waits on it
	      while(j != procNum) {
	        j = (shpalinfo->flag[j] != idle) ? shpalinfo->proc_turn : (j + 1) % shpalinfo->totalProcesses;
	      }


	      //Declare intentions to enter critical section
	      shpalinfo->flag[procNum] = in_cs;
	      //Check that no one else is in the critical section
	      for(j = 0; j < shpalinfo->totalProcesses; j++) {
	        if((j != procNum) && (shpalinfo->flag[j] == in_cs)) {
	          break;
	        }
	      }

	    }while ((j < shpalinfo->totalProcesses) || (shpalinfo->proc_turn != procNum && shpalinfo->flag[shpalinfo->proc_turn] != idle));

	    // Increment shared variable
	    // shpalinfo->sharedInt++;
	    fprintf(stderr,"    Child %d about to enter critical section...\n", procNum + 1);

	    //Assign proc_turn to self and enter critical section
	    shpalinfo->proc_turn = procNum;

	    random = rand() % 2;
	    sleep(random);

	    //** CRITICAL SECTION STARTS HERE **//

	    //check the string for palindrome

	    if(check_palindrome(shpalinfo -> mylist[stringIndex+i])) {
	    	file = fopen(palin_out_filename, "a");
	    }else {
	    	file = fopen(nopalin_out_filename, "a");
	    }

	    if(!file) {
	      perror("    Error opening file");
	      exit(-1);
	    }

	    time_t times = time(NULL);
	    
	    fprintf(file,"    File modified by process number %d at time %lu with stringIndex %d: %s\n", procNum + 1, times, stringIndex+i, shpalinfo->mylist[stringIndex+i]);

	    if(fclose(file)) {
	      perror("    Error closing file");
	    }

	    fprintf(stderr,"    Child %d exiting critical section...\n", procNum + 1);

	    //Exit section
	    j = (shpalinfo->proc_turn + 1) % shpalinfo->totalProcesses;

	    //find the next process that is not idle
	    while(shpalinfo->flag[j] == idle) {
	      j = (j + 1) % shpalinfo->totalProcesses;
	    }

	    //Assign proc_turn to next waiting process and change own flag to idle
	    shpalinfo->proc_turn = j;
	    shpalinfo->flag[procNum] = idle;
	    
	    //Do a random sleep here so that the process idles in "idle mode"
	    //so that other process can randomly take next proc_turn instead of the 
	    //next numbered process
	    random = rand() % 2;
	    sleep(random);

	    i++;
	}

	//Do some final printing based on if all iterations were accomplished
	if(i == max_writes_in) {
		fprintf(stderr, "    Child %d COMPLETED WORK\n", procNum + 1);
	}
	else {
		fprintf(stderr, "    Child %d did NOT complete work\n", procNum + 1);
	}

	if(shmdt(shpalinfo) == -1) {
	    perror("Slave could not detach shared memory");
	}

	kill(myPid, SIGTERM);
  	sleep(1);
  	kill(myPid, SIGKILL);
}

void signal_quit_handler(int sig) {
  printf("    Slave %d has received signal %s (%d)\n", procNum, strsignal(sig), sig);
  signal_flag = 0;
  //The slaves have at most 10 more seconds to exit gracefully or they will be SIGTERM'd
  alarm(KILL_TIMEOUT);
}

//function to kill itself if the alarm goes off,
//signaling that the parent could not kill it off
void alarm_handler(int sig) {
  printf("    Slave %d is killing itself due to slave timeout override\n", procNum);
  kill(myPid, SIGTERM);
  sleep(1);
  kill(myPid, SIGKILL);
}

int check_palindrome(char string_to_check[]) {
	int str_len = strlen(string_to_check);
	int start = 0, end = str_len - 1;
	while( string_to_check[start]==string_to_check[end] && start<=end ) {
		start++;
		end--;
	}
	if(start>end)
	    return 1;
	else 
	    return 0;
}