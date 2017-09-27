#include "shm_header.h"

#define PERM 0777

#define BUFF_SIZE 1024

int main(int argc, char const *argv[])
{
	key_t key;
	int shmid, i;
	pid_t childpid = 0;

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
		// printf("%s\n", buf);
		strcpy(all_strings[pos++], buf);
		num_strings++;
	}

	fclose(fp);

	/* All stings read */
	// fprintf(stderr, "Strings read from input.txt\n");
	// for (i = 0; i < num_strings; ++i)
	// {
	// 	fprintf(stderr, "%s\n", all_strings[i]);
	// }

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
	
		shpinfo -> turn = 1;
		shpinfo -> flag[0] = 9;

		//allocate memory for the char*
		for (i = 0; i < num_strings; ++i)
		{
			shmid = shmget(key, sizeof(char*), IPC_CREAT | 0777);
			shpinfo -> buffer[i] = (char*)shmat(shmid,0,0);
		}
		

		for (i = 0; i < num_strings; ++i)
		{
			shpinfo -> buffer[i] = all_strings[i];	
		}
	}

	for (i = 0; i < num_strings; ++i)
	{
		fprintf(stderr, "%s\n", shpinfo -> buffer[i]);
		// fprintf(stderr, "%d\n", shpinfo -> buffer[i]);
		// fprintf(stderr, "%x\n", shpinfo -> buffer[i]);
		// fprintf(stderr, "%p\n", shpinfo -> buffer[i]);
	}

	for (i = 0; i < 2; ++i)
		if ((childpid = fork()) <= 0)
			break;
	/* child process */
	if(childpid == 0) {
		// fprintf(stderr, "i:%d  process ID:%ld  parent ID:%ld  child ID:%ld\n",
  //          i, (long)getpid(), (long)getppid(), (long)childpid);
		char *palinOptions[] = {"./palin","1", "2", (char *)0};
		execv("./palin", palinOptions);
	} else { /* parent process */
		// fprintf(stderr, "Parent i:%d  process ID:%ld  parent ID:%ld  child ID:%ld\n",
  //          i, (long)getpid(), (long)getppid(), (long)childpid);
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
	}
    

	return 0;
}
