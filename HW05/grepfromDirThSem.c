/*
 *	Doga Mine Kaba
 *	121044004
 *	
 *	This program works for certain sizes which are defined as macros.
 *	(txt input must contain less characters than MAX_RESULT_BUF_SIZE)
 */

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>
#define FIFO_PERM (S_IRUSR | S_IWUSR)
#define MAX_NAME_SIZE 1000
#define MAX_LINE_SIZE 2000
#define MAX_BUF_SIZE 100
#define MAX_THREAD_SIZE 1000
#define MAX_RESULT_BUF_SIZE 50000
#define FIFO_NAME "grepfromDirThSem.fifo"

static volatile int doneFlag = 0;
char givenWord[MAX_NAME_SIZE];

/* checks if given file is valid */
int isTxtFile(char* fileName);
/* searchs directories */
int searchForWord(char* dirName);
/* finds word occurences in a txt file */
void findWords(char* fileName, char* resultBuf);
/* adds information to log file */
int appendToLog(char* line, int size, char* resultBuf, int lineNum, int prevOcc);
int isDir(char* fileName);
int isFifoFile(char* fileName);
void* threadFunc(void *arg);
/* reads character by character until '\n' and returns number of chars */
int readLineFromFIFO(int fd, char * line, size_t size);

struct thrd_data {
    sem_t * lck;
    char arg[MAX_NAME_SIZE];
};

static void setDoneFlag(int signo) {
	doneFlag = 1;
}

int main(int argc, char* argv[]){
	pid_t pid;
	char logName[MAX_NAME_SIZE];
	FILE *mainLogFile;
	char line[MAX_LINE_SIZE];
	int fd;
	char buf[MAX_BUF_SIZE];
	char fifoName[MAX_NAME_SIZE];
	char *totalWordPtr;
	int totalWords = 0;
	
	signal(SIGINT, setDoneFlag);
	
	if(argc != 3) /* argc should be 3 for correct execution */
	{
		printf("\nUsage: %s <dirName> \"string\"\n\n", argv[0]);
		return -1;
	}

	if(!isDir(argv[1]))
	{
		fprintf(stderr, "Given directory is not valid.\n");
		return -1;
	}

	if(mkfifo(FIFO_NAME, FIFO_PERM) < 0){
		fprintf(stderr, "Could not make fifo file.\n");
		return -1;
	}
	
	strcpy(givenWord, argv[2]);
		
	pid = fork();

	/* Child isn't created */
	if (pid < 0)
	{
		fprintf (stderr, "Could not create child.\n");
		return -1;
	}

	/* Child process */
	if (pid == 0)
	{
		searchForWord(argv[1]);
		exit(0);
	}

	/* Parent process */
	fd = open(FIFO_NAME, O_RDONLY);
	

	/* Wait for child process*/
	while(pid = wait(NULL))
	{
		if((pid == -1) && (errno != EINTR))
			break;
	}

	mainLogFile = fopen("gfD.log", "w");
	while (readLineFromFIFO(fd, buf, MAX_BUF_SIZE)>0){
		// *.dmk.* is my code to figure out that current line 
		// holds the total count of occurences
		totalWordPtr = strstr(buf,"*.dmk.*");
		while(totalWordPtr!=NULL){
			if(totalWordPtr != NULL){
				totalWords += atoi(totalWordPtr + 7);
				while((*totalWordPtr)!='\n'){
					*totalWordPtr=' ';
					totalWordPtr+=1;
				}
			}
			totalWordPtr = strstr(buf,"*.dmk.*");
		}
		fprintf(mainLogFile, "%s", buf);
	}
	fprintf(stderr,"\nTotal count of words that found: %d\n\n", totalWords);
	
	if(doneFlag)
		fprintf(mainLogFile, "Program is terminated by CTRL + C signal.\n");
	else
		fprintf(mainLogFile, "Program is terminated successfully.\n");
	
	close(fd);
	fclose(mainLogFile);
	unlink(FIFO_NAME);
	
	return 0;
}

int isTxtFile(char* fileName)
{
	FILE* file = fopen(fileName, "r");
	
	/* To check the extension */	
	int fileNameLen = 0, i;
	char  fileEnd[5];
	
	if ( file == 0 )
		return 0;
	else 
	{
		fclose(file);
		if (strlen(fileName) > 4)
		{
			fileNameLen = strlen(fileName) - 4;
		
			/* Get extension */
			for (i = 0; i < 4; i += 1)
				fileEnd[i] = fileName[fileNameLen + i]; 
			fileEnd[i] = '\0';
			
			if (strcmp(fileEnd, ".txt") == 0)
				return 1;
			else
				return 0;
		}
	}
	return 0;
}

int appendToLog(char* line, int size, char* resultBuf, int lineNum, int prevOcc)
{
	int i=0, j, begin=0, end=0, occurence=prevOcc;
	char subStr[size];
	
	while(line[i] != '\n')
	{
		if(line[i] != ' ')
		{
			/* find the column number for matching string */
			memcpy(subStr, line+i, size);
			subStr[size] = '\0';
			if(strcmp(subStr, givenWord) == 0)
			{
				/*Extra step to print the full string that includes substring*/
				for(j=i; line[j] != ' '; ++j)
					if(line[j] == '\n')
						break;
				end = j;
				
				for(j=i; line[j] != ' '; --j)
					if(j == 0)
					{
						j = -1;
						break;
					}
				begin = j+1;
				
				/* Add occurence information to log file */
				++occurence;
				sprintf(resultBuf + strlen(resultBuf), " %d: \"", occurence);
				
				for(j=begin; j<end; ++j)
					sprintf(resultBuf + strlen(resultBuf), "%c",line[j]);
					
				sprintf(resultBuf + strlen(resultBuf), "\" line: %d column: %d\n", lineNum, i);
			}
		}
		++i;
	}
	return occurence;
}

void findWords(char* fileName, char* resultBuf){
	FILE* fileIn;
	char line[MAX_LINE_SIZE];
	int wordSize=0, i=0, lineNum=0, occurence=0;
	
	fileIn = fopen(fileName, "r");
	
	/* Find the size of given word */
	while(givenWord[i] != '\0')
	{
		++wordSize;
		++i;
	}
	
	/* Add filename and given word to the log file */
	sprintf(resultBuf, "%s\n Given word is: %s\n", fileName, givenWord);
	
	while(fgets(line, sizeof(line), fileIn) != 0)
	{
		/* Append to the log file */
		occurence = appendToLog(line, wordSize, resultBuf, lineNum, occurence);
		++lineNum;
	}
	
	if(occurence <= 0)
		sprintf(resultBuf + strlen(resultBuf), "Word not found.\n");
	
	sprintf(resultBuf + strlen(resultBuf), "*.dmk.*%d\n", occurence);
	
	fclose(fileIn);
}

int searchForWord(char* dirName)
{	
	/* file directory */
	struct dirent* in_file;
	DIR*	FD;
	char	fileName[MAX_NAME_SIZE];
	pid_t	pid;
	pthread_t tid[MAX_THREAD_SIZE];
	int		i, j, threadCount = 0;
	int		totalWords = 0;
	int		err;
	struct thrd_data data[MAX_THREAD_SIZE];
	sem_t semlock;
	int fd;
	
	if(sem_init(&semlock, 0, 1) == -1){
		fprintf(stderr,"Failed to initialize semaphore.\n");
		return totalWords;
	}

	// slow down for 200 miliseconds to allow ctrl + c to the user
	usleep(200000);
	
	//fprintf(stderr,"process: %d\n", getpid());
	
	if (NULL == (FD = opendir(dirName))) 
		return 0;
		
	while ((in_file = readdir(FD)) != NULL && !doneFlag) 
	{
		if(!(strcmp(in_file->d_name,".") == 0 || 
			strcmp(in_file->d_name,"..") == 0))
		{
			strcpy(fileName, dirName);
			strcat(fileName,"/");
			strcat(fileName, in_file->d_name);
			
			if(isFifoFile(fileName))
				continue;
			
			if (isDir(fileName) && !doneFlag)
			{
				pid = fork();
			
				/* Child isn't created */
				if (pid < 0)
				{
					return -1;
				}
			
				/* Child process */
				if (pid == 0)
				{
					closedir(FD);
					searchForWord(fileName);
					exit(0);
				}
			}
			else if (isTxtFile(fileName) && !doneFlag)
			{
				if(threadCount < MAX_THREAD_SIZE){
					strcpy(data[threadCount].arg, fileName);

					data[threadCount].lck = &semlock;
				
					// create thread
					err = pthread_create(&tid[threadCount], NULL, &threadFunc, &data[threadCount]);
					if (err != 0)
					{	
						fprintf(stderr,"\ncan't create thread :[%s]", strerror(err));
					}
					else{
						++threadCount;
					}
				}
				else{
					fprintf(stderr,"\nProgram reached to max thread count. Can't create thread.\n");
					fprintf(stderr,"Program didn't perform process on %s\n\n", fileName);
				}
			}
		}
	}
	
	if(doneFlag){
		kill (getpid(), SIGINT);
	}
	
	// join threads
	for(i=0; i<threadCount; ++i)
	{
		if(err = pthread_join(tid[i], NULL))
			fprintf(stderr, "Failed to join thread: %s\n", strerror(err));
	}
	// Wait child processes of this dir
	while(pid = wait(NULL))
	{
		if((pid == -1) && (errno != EINTR))
			break;
	}
	
	if(threadCount <= 0){
		fd = open(FIFO_NAME, O_WRONLY);	
		close(fd);
	}
	
	sem_destroy(&semlock);
	closedir(FD);
	return totalWords;
}

int isDir(char* fileName)
{
	struct stat st;

	if(!stat(fileName, &st) && S_ISDIR(st.st_mode)){
		return 1;
	}
	return 0;
}

int isFifoFile(char* fileName){
	
	struct stat st;

	if(!stat(fileName, &st) && S_ISFIFO(st.st_mode)){
		return 1;
	}
	return 0;
}

void* threadFunc(void *arg){

	struct thrd_data* d= arg;	
	
	sem_t* semlockp = d->lck;

	char foundWords[MAX_RESULT_BUF_SIZE];

	int tid=pthread_self();
	int i=0;
	int fd=0;
	
	findWords(d->arg, foundWords);

	while(sem_wait(semlockp) == -1)
		if(errno != EINTR){
			fprintf(stderr, "Thread failed to lock semaphore.\n");
			return NULL;
		}
	
	fd = open(FIFO_NAME, O_WRONLY);

	if(write(fd, foundWords, strlen(foundWords)) == -1){
		fprintf(stderr, "%d couldn't write to main fifo\n", tid);
	}
	
	close(fd);
	
	if(sem_post(semlockp) == -1)
		fprintf(stderr, "Thread failed to unlock semaphore.\n");
	
	pthread_exit(NULL);
}

int readLineFromFIFO(int fd, char * line, size_t size) 
{
	char c = 0;
	size_t i = 0;
	if ((NULL != line) && (0 < size))
	{
		while (read(fd, &c, 1) >0) 
		{
			if (('\n' == c) || (size < i)) { 
				line[i] = c;
				++i;
				break;
			}
			line[i] = c;
			++i;
		}
		line [i] = 0;
	}
	return i;
}






