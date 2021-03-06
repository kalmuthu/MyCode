#include <stdio.h>
#include <stdlib.h>
int UtilGetActivePort()
{
	FILE *fptr;
	char buff[128];
	char *ptr = NULL;
	
	/*
		"r"		Open a file for reading. The file must exist.
		"w"		Create an empty file for writing. If a file with the same name 
				already exists its content is erased and the file is treated as a new empty file.
		"a"		Append to a file. Writing operations append data at the end of the file. 
				The file is created if it does not exist.
		"r+"	Open a file for update both reading and writing. The file must exist.
		"w+"	Create an empty file for both reading and writing. If a file with the same name 
				already exists its content is erased and the file is treated as a new empty file.
		"a+"	Open a file for reading and appending. All writing operations are performed at the end of the file,
				protecting the previous content to be overwritten. 
				You can reposition (fseek, rewind) the internal pointer to anywhere in the file for reading,
				but writing operations will move it back to the end of file. 
				The file is created if it does not exist.
	*/
	
	fptr = fopen("/proc/net/bonding/bond0", "r")
	
	while ( !feof(fptr))
	{
		fgets(buff,128,fptr);
		if ( strstr(buff,"Currently Active Slave") != NULL)
		{
			ptr = strstr(buff,"eth");
			break;	
		}
	}
	if (ptr)
	{
		printf ("1%s1",ptr);
	}
	fclose(fptr);
	return ptr;
}