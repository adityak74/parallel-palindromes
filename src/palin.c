#include "shm_header.h"
#define PERM 0777

int main(int argc, char const *argv[])
{

	if(argc != 3){
		fprintf(stderr, "Not enough arguments\n");
		return -1;
	}

	key_t key = SHM_KEY;
	int shmid, i;

	shared_palinfo *shpalinfo;

	shmid = shmget(key, sizeof(shared_palinfo), PERM);
	fprintf(stderr, "%d\n", shmid);
	if(shmid == -1) {
		if (((shmid = shmget(key, sizeof(shared_palinfo), PERM)) == -1) || 
			(shpalinfo = (shared_palinfo*)shmat(shmid, NULL, 0) == (void *)-1) )
			return -1;
	} else {
		shpalinfo = shmat(shmid, NULL, 0);
		if(shpalinfo == (void *)-1)
			return -1;

		fprintf(stderr, "%d\n", shpalinfo -> turn);
		fprintf(stderr, "%d\n", shpalinfo -> flag[0]);

		if(shmdt(shpalinfo) == -1) {
		    perror("Slave could not detach shared memory");
		 }

		// fprintf(stderr, "%d\n", shpalinfo -> buffer[0]);
		// fprintf(stderr, "%x\n", shpalinfo -> buffer[0]);
		// fprintf(stderr, "%p\n", shpalinfo -> buffer[0]);

		// char* ptr;
		// *ptr = (char *)shpalinfo -> buffer[0];
		// char arr[100];
		// int pos = 0;
		// while((*ptr) != '\0') {
		// 	// fprintf(stderr, "%c", *ptr);
		// 	arr[pos++] = *ptr;
		// 	ptr = ptr + 1;
			
		// }
		// arr[pos] = '\0';
		// fprintf(stderr, "%s\n", arr);

		// for (i = 0; i < 4; ++i)
		// {
		// 	fprintf(stderr, "%s\n", shpalinfo -> buffer[0]);
		// }
		
	}

	return 0;
}