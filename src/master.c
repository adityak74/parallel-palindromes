#include "shm_info.h"

#define PERM (S_IRUSR | S_IWUSR)

int main(int argc, char const *argv[])
{
	key_t key;
	int shmid;

	struct shared_palinfo *shpinfo;

	key = ftok(".", 'c');
	if(key == (key_t)-1) {
		fprintf(stderr, "Failed to derive key\n");
	}

	printf("%x\n", key);

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
			return -1;
		shpinfo -> turn = 1;
		shpinfo -> flag[0] = 9;
	}

	exec("./palin");

	return 0;
}
