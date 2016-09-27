
#include <asm/segment.h>
#include <errno.h>

#define NAME_LEN 24
char saved_name[NAME_LEN];

int sys_iam(const char * name)
{
	int count = 0;
	int name_length = 0;
	while(get_fs_byte(name+count)!='\0')
	{
		count++;

		if (count>=NAME_LEN)
		{
			return -EINVAL;
		}
	}
	name_length=count;
	for (count=0; count<name_length; count++)
	{
		saved_name[count]=get_fs_byte(name+count);
	}
	saved_name[count]='\0';
	return name_length;
}

int sys_whoami(char* name, unsigned int size)
{
	int count = 0;
	if (size==0)
	{
		return -EINVAL;
	}

	while(saved_name[count]!='\0')
	{

		put_fs_byte(saved_name[count], (name+count));
		count++;

		if (count>=size)
		{
			return -EINVAL;
		}
	}
	put_fs_byte(saved_name[count], (name+count));
	return count;
}

