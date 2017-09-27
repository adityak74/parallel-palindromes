#include <string.h>
#include <stdio.h>


#define BUFF_SIZE 1024

int main(int argc, char const *argv[])
{
	
	char* mylist_orig[4] = { "hellloweowoe",
						"jsdfhn",
						"dfjnhdkjfnhdf",
						"sfhgdj"};

	char *buffer[4];
	int i;
	for (i = 0; i < 4; ++i)
	{
		buffer[i] = mylist_orig[i];
	}

	// fprintf(stderr, "%s\n", buffer[0]);
	
	char* ptr = buffer[1];
	char arr[100];
	int pos = 0;
	while(*ptr != '\0') {
		// fprintf(stderr, "%c", *ptr);
		arr[pos++] = *ptr;
		ptr = ptr + 1;
		
	}
	arr[pos] = '\0';
	fprintf(stderr, "%s\n", arr);


	char all_strings[BUFF_SIZE][BUFF_SIZE];
	int num_strings = 0;
	pos = 0;
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

	for (i = 0; i < num_strings; ++i)
	{
		fprintf(stderr, "%s\n", all_strings[i]);
	}

	char* nptr = all_strings[0];
	fprintf(stderr, "%p\n", nptr);
	// fprintf(stderr, "%c\n", *nptr);

	fclose(fp);
	while(1);
	return 0;
}