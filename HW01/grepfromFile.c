/*
 *	Doga Mine Kaba
 *	121044004
 *	
 */

#include <stdio.h>
#include <string.h>

/* to count occurences of the given word */
int occurence = 0;
/* checks if given file is valid */
int txtFileExists(char* fileName);
/* adds information to log file */
void appendToLog(char* line, char* word, int size, FILE* log, int lineNum);

int main(int argc, char* argv[]){
	FILE* fileIn;
	FILE* logfile;
	char line[2000];
	int wordSize=0, i=0, lineNum=0;
	
	if(argc != 3) /* argc should be 3 for correct execution */
	{
		printf("\nUsage: %s filename.txt word\n\n", argv[0]);
		return -1;
	}
	else
	{
		if(txtFileExists(argv[1]))
		{
			fileIn = fopen(argv[1], "r");
			logfile = fopen("gfF.log", "a");
			
			/* Find the size of given word */
			while(argv[2][i] != '\0')
			{
				++wordSize;
				++i;
			}
			
			/* Add filename and given word to the log file */
			fprintf(logfile, "%s\n", argv[1]);
			fprintf(logfile, " Given word is: %s\n", argv[2]);
			printf("%s\n", argv[1]);
			printf(" Given word is: %s\n", argv[2]);
			
			while(fgets(line, sizeof(line), fileIn) != 0)
			{
				/* Append to the log file */
				appendToLog(line, argv[2], wordSize, logfile, lineNum);
				++lineNum;
			}
			
			if(occurence <= 0)
			{
				fprintf(logfile, " Word not found. \n");
				printf(" Word not found. \n");
			}
			
			fprintf(logfile, "\n");
			printf("\n");
			
			fclose(logfile);
			fclose(fileIn);
		}
	}
}

int txtFileExists(char* fileName)
{
	FILE* file = fopen(fileName, "r");
	
	/* To check the extension */	
	int fileNameLen = 0, i;
	char  fileEnd[5];
	
	if ( file == 0 )
	{
		printf("File not found.\n");
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
				printf("This file is not a txt file.\n");
				return 0;
			}
		}
	}
	printf("File not found.\n");
	return 0;
}

void appendToLog(char* line, char* word, int size, FILE* log, int lineNum)
{
	int i=0, j, begin=0, end=0;
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
				printf(" %d: \"", occurence);
				
				for(j=begin; j<end; ++j)
				{
					fprintf(log,"%c",line[j]);
					printf("%c",line[j]);
				}
				fprintf(log,"\" line: %d column: %d\n", lineNum, i);
				printf("\" line: %d column: %d\n", lineNum, i);
			}
		}
		++i;
	}
}

