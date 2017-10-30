/*
 *	Doga Mine Kaba
 *	121044004
 *	
 *	This program works for certain sizes
 *	which are defined as macros.
 */
 
 
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#define SERVER_FIFO "integralgen.fifo"
#define MAX_BUF_SIZE 1000
#define MAX_NAME_SIZE 1000

static volatile int doneFlag = 0;

int argumentsValid(char** arguments);
static void setDoneFlag(int signo) {
    doneFlag = 1;
}

int main(int argc, char* argv[]){

	int fdw, fdr=-1;
	char buf[MAX_BUF_SIZE];
	char fifoName[MAX_NAME_SIZE];
	int readsize=0;
	char forkPid[MAX_BUF_SIZE];
	FILE* logfile;
	char logname[MAX_NAME_SIZE];
	int success = 0;
	
	signal(SIGINT, setDoneFlag);

	if(argc != 5) /* argc should be 5 for correct execution */
	{
		printf("\nUsage: %s <fi> <fj> <time interval> <operation> \n", argv[0]);
		printf("Note: don't forget to use * operation as \"*\" to avoid confusion.\n\n");
		return -1;
	}
	if(argumentsValid(argv) == -1){
		printf("Program terminated.\n");
		return -1;
	}
	
	sprintf(logname,"%d.log",getpid());
	logfile = fopen(logname, "w");
	
	fprintf(logfile, "function1: %s operation: %s function2: %s\n", argv[1], argv[4], argv[2]);
	
	sprintf(buf, "%d %s %s %s %s\n", getpid(), argv[1], argv[2], argv[3], argv[4]);
	
	fdw = open(SERVER_FIFO, O_WRONLY | O_NONBLOCK);
	if(fdw == -1)
		printf("Server is not open. Waiting for server...\n");
	while(fdw == -1)
		fdw = open(SERVER_FIFO, O_WRONLY | O_NONBLOCK);
	
	if(write(fdw, buf, strlen(buf)) == -1){
		fprintf(stderr, "Couldn't write to %s\n", SERVER_FIFO);
	}
	else
		printf("Connected. Waiting for respond...\n");
	
	sprintf(fifoName, "%d.fifo", getpid());
	
	// wait until respond fifo is created
	while(fdr == -1)
		fdr = open(fifoName, O_RDONLY | O_NONBLOCK);

	while (!doneFlag){
		if((readsize = read(fdr, buf, MAX_BUF_SIZE))>0){
			buf[readsize]='\0';
			if(strstr(buf, "pid:")){
				strcpy(forkPid,strtok(buf, "pid:"));
			}
			else if(strstr(buf, "error: ")){
				printf("%s", buf);
				break;
			}
			else if(strstr(buf, "end.")){
				success = 1;
				break;
			}
			else
				fprintf(logfile,"%s",buf);
		}
	}
	
	if(doneFlag)
		fprintf(logfile,"Program is terminated by Ctrl C.\n");
	else if(success)
		fprintf(logfile,"Program is terminated successfully.\n");
	else
		fprintf(logfile,"Program is terminated by an error.\n");
		
	printf("Program is terminated.\n");

	//printf("fork pid: %d\n",atoi(forkPid));
	kill (atoi(forkPid), SIGINT);
	
	close(fdw);
	close(fdr);
	fclose(logfile);
	unlink(fifoName);
	return 0;
}

int argumentsValid(char** arguments){
	int result = 1;
	int i;
	char filename[MAX_NAME_SIZE];

	for(i=0; i < strlen(arguments[3]); ++i){
		if(arguments[3][i] > '9' || arguments[3][i] < '0'){
			fprintf(stderr, "%s is not a valid number.\n", arguments[3]);
			result = -1;
			break;
		}
	}
	if(!(arguments[4][0] == '+' || arguments[4][0] == '-' 
	|| arguments[4][0] == '*' || arguments[4][0] == '/')){
		fprintf(stderr, "Unsupported operation: %s\n", arguments[4]);
		result = -1;
	}
	return result;
}






