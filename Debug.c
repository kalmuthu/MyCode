#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/stat.h>



#define MUTEX_ID  0x691206

#define MAX_LINE_LEN   128
#define MAX_FILE_SIZE  50
#define LOG_FILE "message"


/*
 	   In some earlier versions of glibc, the semun union was defined in <sys/sem.h>,
       but POSIX.1-2001 requires that the caller define this union.  On versions of
       glibc where this union is not defined, the macro _SEM_SEMUN_UNDEFINED is
       defined in <sys/sem.h>.
*/
#if defined ( _SEM_SEMUN_UNDEFINED )
union semun {
	int val; 					/* value for SETVAL */
	struct semid_ds *buf; 		/* buffer for IPC_STAT, IPC_SET */
	unsigned short int *array; 	/* array for GETALL, SETALL */	
	struct seminfo *__buf;		/* buffer for IPC_INFO */
};
#endif

int mutex_create()
{
	int     	   mutex_id;
	union semun    sem_arg;
	
	// the options IPC_CREAT|IPC_EXCL :prevent key from duplicating
	mutex_id = semget((key_t)MUTEX_ID , 1, IPC_CREAT|IPC_EXCL|0666);
	if (mutex_id ==  -1)
	{
		if (errno == EEXIST)	
		{
			mutex_id = semget((key_t)MUTEX_ID , 1, IPC_CREAT|0666);
		}
		else 
			return -1;
	}
	//initialize mutex
	sem_arg.val = 1;
	semctl(mutex_id, 0, SETVAL, sem_arg);
	return mutex_id;
}
int mutex_lock(int id)
{
	struct sembuf  sem_buf = {0 ,-1 ,SEM_UNDO};	
	
	if (semop(id, &sem_buf, 1) < 0 )
	{
		 perror("semop");
		 return -1;
	}
	return 0;	
}
int mutex_unlock(int id)
{
	struct sembuf  sem_buf = {0 ,1 ,SEM_UNDO};	
	
	if (semop(id, &sem_buf, 1) < 0 )
	{
		 perror("semop");
		 return -1;
	}
	return 0;	
}

int DEBUG_LOG(const char *format,...)
{
	va_list 	va_l;
	char    	*p,*np;
	int 		n,size;
	FILE    	*fptr;
	int 		mutex_id;
	struct stat buf;
	
	mutex_id = mutex_create();
	size  = MAX_LINE_LEN;
	if ((p = malloc(size)) == NULL )
	{
		printf ("can' alloacte memory (size %d)\n",size);
		return 0;	
	}
	while (1)
	{
		va_start(va_l,format);
		n = vsnprintf(p,size,format,va_l);
		/* If that worked, return the string. */
        if (n > -1 && n < size)
            break;
        /* Else try again with more space. */
        if (n > -1)    /* glibc 2.1 */
           size = n+1; /* precisely what is needed */
        else           /* glibc 2.0 */
           size *= 2;  /* twice the old size */
        if ((np = realloc (p, size)) == NULL) 
        {
            free(p);
           	printf ("can' alloacte memory (size %d)\n",size);
			return -1;
        } 
        else 
        {
            p = np;
        }
	}
	va_end(va_l);
	
	if ( (fptr = fopen(LOG_FILE, "r+")) == NULL)
	{
		if ((fptr = fopen(LOG_FILE, "w+")) == NULL)
		{
			printf("can't open log file %s\n ",LOG_FILE);
			return -1;
		}
	}
	
	fstat(fileno(fptr),&buf);
	if ( buf.st_size >= MAX_FILE_SIZE)
	{
		fseek(fptr,0,SEEK_SET);
		at_dw_debug_offset = 0; 
		printf ("debug buffer is full\n");
	}
	else 
	{
		fseek(fptr,at_dw_debug_offset,SEEK_SET);
	}
	mutex_lock(mutex_id);
	fputs (p,fptr);
	mutex_unlock(mutex_id);
	fclose(fptr);
	return 0;
}

int main()
{
	
	DEBUG_LOG("dasdsa");
}