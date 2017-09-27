#include <string.h>
#include <stdio.h>

#define BUFF_SIZE 1024

int main(int argc, char const *argv[])
{
	
	
	
	char* ptr = (char *)0x7fff51242670;
	char arr[100];
	int pos = 0;
	fprintf(stderr, "%c", *ptr);
	while(*ptr != '\0') {
		fprintf(stderr, "%c", *ptr);
		arr[pos++] = *ptr;
		ptr = ptr + 1;
		
	}
	// arr[pos] = '\0';
	// fprintf(stderr, "%s\n", arr);

	return 0;
}