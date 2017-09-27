#include "shm_header.h"
#define PERM (S_IRUSR | S_IWUSR)

int main(int argc, char const *argv[])
{

	// if(argc != 3){
	// 	fprintf(stderr, "Not enough arguments\n");
	// 	return -1;
	// }

	key_t key = 0x6302c6be;
	int shmid, i;

	shared_palinfo *shpinfo;

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

		for (i = 0; i < 4; ++i)
		{
			fprintf(stderr, "%s\n", shpinfo -> buffer[0]);
		}
		
	}

	return 0;
}