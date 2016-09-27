/**
 * iam.c
 * the function call in file system
 */
#include <who.h>

void main(int argc, char **argv)
{
	int result;
	if (argc==1)
	{
		result=iam("");
		printf("input empty string, output %d\n", result);
	}
	else
	{
		result=iam((char *)argv[1]);
		printf("input %s, output %d\n", (char *)argv[1], result);
	}
}
