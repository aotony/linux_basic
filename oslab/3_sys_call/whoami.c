/**
 * iam.c
 * the function call in file system
 */
#include <who.h>

void main(void)
{
	char output[24];
	int result;
	result=whoami(output, 24);
	printf("%s\n", output);
}
