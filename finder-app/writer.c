#include <syslog.h>
#include <stdio.h>


// argv[1] -> file
// argv[2] -> string

int main(int argc, char const *argv[])
{
	openlog(NULL, 0, LOG_USER);
	if (argc < 3)
	{
		syslog(LOG_ERR, "writefile or writestr does not specified");
		return 1;
	}
	else
	{
		syslog(LOG_DEBUG, "Writing %s to %s",argv[2] ,argv[1]);
		FILE *fptr;
		fptr = fopen(argv[1], "w");
		fprintf(fptr, argv[2]);
		fclose(fptr);
	}
	return 0;
}