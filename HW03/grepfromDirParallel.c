/*
 *	Doga Mine Kaba
 *	121044004
 *	
 *	This program works for certain sizes
 *	which are defined as macros.
 */

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
#define FIFO_PERM (S_IRUSR | S_IWUSR)
#define MAX_NAME_SIZE 1000
#define MAX_LINE_SIZE 2000
#define MAX_BUF_SIZE 10000
#define MAX_PIPE_SIZE 1000
#define MAX_FIFO_SIZE 1000

static volatile int doneFlag = 0;

/* checks if given file is valid */
int isTxtFile(char* fileName);
/* searchs directories */
int searchForWord(char* dirName, char* word);
/* finds word occurences in a txt file */
void findWords(char* fileName, char* word, int logName);
/* adds information to log file */
int appendToLog(char* line, char* word, int size, FILE* log, int lineNum, int prevOcc);
int isDir(char* fileName);
int readFromPipeAddFIFO (int file, char* fifoName);

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

	strcpy(fifoName, argv[1]);
	strcat(fifoName,".fifo");
	if(mkfifo(fifoName, FIFO_PERM) < 0){
		fprintf(stderr, "Could not make fifo file.\n");
		return -1;
	}
		
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
		searchForWord(argv[1], argv[2]);
		exit(0);
	}

	/* Parent process */
	fd = open(fifoName, O_RDONLY);
	

	/* Wait for child process*/
	while(pid = wait())
	{
		if((pid == -1) && (errno != EINTR))
			break;
	}

	mainLogFile = fopen("gfD.log", "w");
	while (read(fd, buf, MAX_BUF_SIZE)>0) {
		totalWordPtr = strstr(buf,"*.dmk.*");
		if(totalWordPtr != NULL){
			totalWords = atoi(totalWordPtr + 7);
			*totalWordPtr = '\0';
		}
		fprintf(mainLogFile, "%s", buf);
		fprintf(stderr,"\nTotal count of words that found: %d\n\n", totalWords);
	}
	
	if(doneFlag){
		fprintf(mainLogFile, "Program is terminated by CTRL + C signal.\n");
	}

	close(fd);
	fclose(mainLogFile);
	// For any informaiton loss
	strcpy(fifoName, argv[1]);
	strcat(fifoName,".fifo");
	unlink(fifoName);
	
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

int appendToLog(char* line, char* word, int size, FILE* log, int lineNum, int prevOcc)
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
			if(strcmp(subStr, word) == 0)
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
				fprintf(log," %d: \"", occurence);
				
				for(j=begin; j<end; ++j)
					fprintf(log,"%c",line[j]);
				fprintf(log,"\" line: %d column: %d\n", lineNum, i);
			}
		}
		++i;
	}
	return occurence;
}

void findWords(char* fileName, char* word, int logName){
	FILE* fileIn;
	FILE* logfile;
	char line[MAX_LINE_SIZE];
	int wordSize=0, i=0, lineNum=0, occurence=0;
	
	fileIn = fopen(fileName, "r");
	logfile = fdopen(logName, "w");
	
	/* Find the size of given word */
	while(word[i] != '\0')
	{
		++wordSize;
		++i;
	}
	
	/* Add filename and given word to the log file */
	fprintf(logfile, "%s\n", fileName);
	fprintf(logfile, " Given word is: %s\n", word);
	
	while(fgets(line, sizeof(line), fileIn) != 0)
	{
		/* Append to the log file */
		occurence = appendToLog(line, word, wordSize, logfile, lineNum, occurence);
		++lineNum;
	}
	
	if(occurence <= 0)
	{
		fprintf(logfile, " Word not found. \n");
	}
	
	fprintf(logfile, "*.dmk.*%d\n", occurence);
	
	fprintf(logfile, "\n");
	
	fclose(logfile);
	fclose(fileIn);
}

int searchForWord(char* dirName, char* word)
{	
	/* file directory */
	struct dirent* in_file;
	DIR*	FD;
	char	fileName[MAX_NAME_SIZE];
	char	dirFifo[MAX_NAME_SIZE];
	char	fileFifo[MAX_NAME_SIZE];
	pid_t	pid;
	int		mypipe[MAX_PIPE_SIZE][2];
	int		fd, fdr, fdw;
	char	buf[MAX_BUF_SIZE];
	char	*totalWordPtr=NULL;
	int		i, j, pipeCount = 0, fifoCount = 0;
	int		fdArr[MAX_FIFO_SIZE];
	char	fifoNames[MAX_FIFO_SIZE][MAX_NAME_SIZE];
	int		totalWords = 0;
	
	// slow down for 200 miliseconds to allow ctrl + c to the user
	usleep(200000);
	
	strcpy(dirFifo, dirName);
	strcat(dirFifo,".fifo");
	
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
			
			if (isDir(fileName))
			{
				strcpy(fileFifo, fileName);
				strcat(fileFifo,".fifo");
				if(mkfifo(fileFifo, FIFO_PERM) < 0){
					fprintf(stderr, "Could not make fifo file: %s\n", fileFifo);
				}
				else{
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
						searchForWord(fileName, word);
						exit(0);
					}
					else{
						fdr = open(fileFifo, O_RDONLY);
						if(fdr == -1)
							fprintf(stderr, "couldn't open %s to read\n", fileFifo);
						else
						{
							strcpy(fifoNames[fifoCount], fileFifo);
							fdArr[fifoCount] = fdr;
							++fifoCount;
						}
					}
				}
			}
			else if (isTxtFile(fileName))
			{
				if (pipe (mypipe[pipeCount]))
				{
					fprintf (stderr, "Pipe failed.\n");
					return EXIT_FAILURE;
				}
				
				pid = fork();
				
				/* Child isn't created */
				if (pid < 0)
				{
					fprintf (stderr, "Couldn't create child.\n");
					return -1;
				}
				
				/* Child process */
				if (pid == 0)
				{
					closedir(FD);
					// Close other end first
					close (mypipe[pipeCount][0]);
					findWords(fileName, word, mypipe[pipeCount][1]);
					close (mypipe[pipeCount][1]);
					exit(0);
				}
				/* parent process */
				else 
				{
					// Close other end
					close (mypipe[pipeCount][1]);
				}
				++pipeCount;
			}
		}
	}
	
	if(doneFlag){
		kill (getpid(), SIGINT);
	}
	// Wait child processes of this dir
	while(pid = wait())
	{
		if((pid == -1) && (errno != EINTR))
			break;
	}

	// Read from pipes and add to dirName fifo
	for(i=0; i<pipeCount; ++i)
	{
		totalWords += readFromPipeAddFIFO (mypipe[i][0], dirFifo);
		close (mypipe[i][0]);
	}


	fdw = open(dirFifo, O_WRONLY);

	// Read from subdir fifos add to dir fifo
	for(i=0; i<fifoCount; ++i)
	{
		fdr = fdArr[i];
	
		while (read(fdr, buf, MAX_BUF_SIZE)>0) 
		{
			// *.dmk.* is my code to figure out that current line 
			// holds the total count of occurences
			totalWordPtr = strstr(buf,"*.dmk.*");
			if(totalWordPtr != NULL){
				totalWords += atoi(totalWordPtr + 7);
				*totalWordPtr = '\0';
			}
			if(write(fdw, buf, strlen(buf)) == -1){
				fprintf(stderr, "couldn't write to %s\n", dirFifo);
			}
		}
		close(fdr);
		unlink(fifoNames[i]);
	}
	sprintf(buf, "*.dmk.*%d\n", totalWords);
	if(write(fdw, buf, strlen(buf)) == -1){
		fprintf(stderr, "couldn't write to %s\n", dirFifo);
	}

	if(fifoCount == 0 && pipeCount==0){
		fd = open(dirFifo, O_WRONLY);
		write(fd, "\0", 1);
		close(fd);
	
		close(fdw);
		closedir(FD);
	}
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

int readFromPipeAddFIFO (int file, char* fifoName)
{
	FILE *stream;
	char c;
	int i=0;
	char buf[MAX_BUF_SIZE];
	char *totalWordPtr;
	int fd;
	int totalWords = 0;

	stream = fdopen (file, "r");
	
	for(i=0; i<MAX_BUF_SIZE && ((c = fgetc (stream)) != EOF); ++i)
	{
		buf[i] = c;
	}
	buf[i] = '\0';
	totalWordPtr = strstr(buf,"*.dmk.*");
	if(totalWordPtr != NULL){
		totalWords = atoi(totalWordPtr + 7);
		*totalWordPtr = '\n';
		*(totalWordPtr+1) = '\0';
	}
	
	fd = open(fifoName, O_WRONLY);
	if(fd == -1)
		fprintf(stderr, "couldn't open %s\n", fifoName);
	
    if(write(fd, buf, strlen(buf)) == -1){
    	fprintf(stderr, "couldn't write to %s\n", fifoName);
    }
    
    close(fd);
	fclose (stream); 
	return totalWords;
}
