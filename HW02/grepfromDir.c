/*
 *	Doga Mine Kaba
 *	121044004
 *	
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#define MAX_NAME_SIZE 256
#define MAX_LINE_SIZE 2000
#define MAX_LOG_SIZE 500

/* checks if given file is valid */
int isTxtFile(char* fileName);
/* searchs directories */
int searchForWord(char* dirName, char* word, int pidArr[]);
/* finds word occurences in a txt file */
void findWords(char* fileName, char* word, char* logName);
/* adds information to log file */
int appendToLog(char* line, char* word, int size, FILE* log, int lineNum, int prevOcc);
int isFolder(char* fileName);
/* size of the array which holds the process ids to access log files */
int logSize = 0;

int main(int argc, char* argv[]){
	pid_t pid;
	int pidArr[MAX_LOG_SIZE], i;
	char logName[MAX_NAME_SIZE];
	FILE *mainLogFile, *fileIn;
	char line[MAX_LINE_SIZE];
	
	if(argc != 3) /* argc should be 3 for correct execution */
	{
		printf("\nUsage: %s <dirName> \"string\"\n\n", argv[0]);
		return -1;
	}
	else
	{
		
		searchForWord(argv[1], argv[2], pidArr);
		
		sleep(1); // wait for child processes to be created
		
		/* wait for all child proccesses*/
		while(pid = wait())
		{
			if((pid == -1) && (errno != EINTR))
				break;
		}
		
		mainLogFile = fopen("gfD.log", "w");
		
		/* clean other log files */
		for(i=0; i<logSize; ++i)
		{
			sprintf(logName, "%d", pidArr[i]);
			strcat(logName, ".log");
			fileIn = fopen(logName, "r");
			while(fgets(line, sizeof(line), fileIn) != 0)
			{
				/* Append to the log file */
				fprintf(mainLogFile, "%s", line);
				printf("%s", line);
			}
			fclose(fileIn);
			remove(logName);
		}
		fclose(mainLogFile);
	}
	return 0;
}

int isTxtFile(char* fileName)
{
	FILE* file = fopen(fileName, "r");
	
	/* To check the extension */	
	int fileNameLen = 0, i;
	char  fileEnd[5];
	
	if ( file == 0 )
	{
		//printf("File not found.\n");
		return 0;
 	}
    else 
    {
    	fclose(file);
    	if (strlen(fileName) > 4)
		{
			fileNameLen = strlen(fileName) - 4;
		
			/* get extension */
			for (i = 0; i < 4; i += 1)
				fileEnd[i] = fileName[fileNameLen + i]; 
			fileEnd[i] = '\0';
				    
			if (strcmp(fileEnd, ".txt") == 0)
				return 1;
			else
			{
				//printf("This file is not a txt file.\n");
				return 0;
			}
		}
	}
	//printf("File not found.\n");
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
				//printf(" %d: \"", occurence);
				
				for(j=begin; j<end; ++j)
				{
					fprintf(log,"%c",line[j]);
					//printf("%c",line[j]);
				}
				fprintf(log,"\" line: %d column: %d\n", lineNum, i);
				//printf("\" line: %d column: %d\n", lineNum, i);
			}
		}
		++i;
	}
	return occurence;
}

void findWords(char* fileName, char* word, char* logName){
	FILE* fileIn;
	FILE* logfile;
	char line[MAX_LINE_SIZE];
	int wordSize=0, i=0, lineNum=0, occurence=0;
	
	fileIn = fopen(fileName, "r");
	logfile = fopen(logName, "w");
	
	/* Find the size of given word */
	while(word[i] != '\0')
	{
		++wordSize;
		++i;
	}
	
	/* Add filename and given word to the log file */
	fprintf(logfile, "%s\n", fileName);
	fprintf(logfile, " Given word is: %s\n", word);
	//printf("%s\n", fileName);
	//printf(" Given word is: %s\n", word);
	
	while(fgets(line, sizeof(line), fileIn) != 0)
	{
		/* Append to the log file */
		occurence = appendToLog(line, word, wordSize, logfile, lineNum, occurence);
		++lineNum;
	}
	
	if(occurence <= 0)
	{
		fprintf(logfile, " Word not found. \n");
		//printf(" Word not found. \n");
	}
	
	fprintf(logfile, "\n");
	//printf("\n");
	
	fclose(logfile);
	fclose(fileIn);
}

int searchForWord(char* dirName, char* word, int pidArr[])
{	
	/* file directory */
	struct dirent* in_file;
	DIR*	FD;
	FILE*	inp_file;
	char*	fileName;
	char logName[MAX_NAME_SIZE];
	pid_t	pid;
	
	if (NULL == (FD = opendir(dirName))) 
    {
    	//printf("%s is not valid.\n", dirName);
        return 0;
    }
    
	while ((in_file = readdir(FD)) != NULL ) 
	{
		if(!(strcmp(in_file->d_name,".") == 0 || 
			strcmp(in_file->d_name,"..") == 0))
		{
			fileName = (char*)malloc(MAX_NAME_SIZE*sizeof(char));
			
			strcpy(fileName, dirName);
			strcat(fileName,"/");
			strcat(fileName, in_file->d_name);
			
			if (isFolder(fileName))
			{
				//printf("Folder name: %s\n", fileName);
				searchForWord(fileName, word, pidArr);
			}
			else if (isTxtFile(fileName))
			{
				//printf("Txt name: %s\n", fileName);
				pid = fork();
				/* child process */
				if (pid == 0)
				{
					//printf("pid: %d\n", getpid());
					sprintf(logName, "%d", getpid());
					strcat(logName, ".log");
					findWords(fileName, word, logName);
					exit(0);
				}	
				/* parent process */
				else 
					pidArr[logSize] = pid;
					++logSize;	
			}		
			free(fileName);
		}
	}
	closedir(FD);	
	return 1;
}

int isFolder(char* fileName)
{
	DIR *FD;
	
	FD = opendir(fileName);
	
	if (FD == NULL)
		return 0;
	else
	{
		closedir(FD);
		return 1;
	}
}

