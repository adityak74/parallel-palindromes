#include "shm_header.h"

#define PERM (S_IRUSR | S_IWUSR)

int main(int argc, char const *argv[])
{
	key_t key;
	int shmid, i;
	pid_t childpid = 0;

	char* mylist_orig[4] = { "hellloweowoe",
						"jsdfhn",
						"dfjnhdkjfnhdf",
						"sfhgdj"};

	shared_palinfo *shpinfo;

	key = ftok(".", 'c');
	if(key == (key_t)-1) {
		fprintf(stderr, "Failed to derive key\n");
	}

	// printf("%x\n", key);

	shmid = shmget(key, sizeof(shared_palinfo), PERM | IPC_CREAT | IPC_EXCL);
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

		for (i = 0; i < 4; ++i)
		{
			shpinfo -> buffer[i] = mylist_orig[i];	
		}
	}

	for (i = 0; i < 4; ++i)
	{
		fprintf(stderr, "%s\n", shpinfo -> buffer[i]);
	}

	for (i = 0; i < 2; ++i)
		if ((childpid = fork()) <= 0)
			break;
	/* child process */
	if(childpid == 0) {
		fprintf(stderr, "i:%d  process ID:%ld  parent ID:%ld  child ID:%ld\n",
           i, (long)getpid(), (long)getppid(), (long)childpid);
	} else { /* parent process */
		fprintf(stderr, "Parent i:%d  process ID:%ld  parent ID:%ld  child ID:%ld\n",
           i, (long)getpid(), (long)getppid(), (long)childpid);
	}
    while(1);
	return 0;
}
