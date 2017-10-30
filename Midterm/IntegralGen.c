/*
 *	Doga Mine Kaba
 *	121044004
 *	
 *	An external library named matheval should be installed for parsing
 *	with the command below:
 *	sudo apt-get install libmatheval-dev
 *
 *	This program works for certain sizes
 *	which are defined as macros.
 */
 
 
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <matheval.h>
#include <math.h>
#define FIFO_PERM (S_IRUSR | S_IWUSR)
#define MAX_BUF_SIZE 1000
#define MAX_NAME_SIZE 1000
#define MAX_FUNC_SIZE 2000
#define SERVER_FIFO "integralgen.fifo"
#define MILLION 1000000L

static volatile int doneFlag = 0;
int maxClient=0;
int currentClient=0;

int argumentsValid(char** arguments);
int txtExists(char* fileName);
int readLine(int fd, char * buffer, size_t size);
double calculateIntegral(char func[], long t0, long res);

static void setDoneFlag(int signo) {
    doneFlag = 1;
}

int main(int argc, char* argv[]){
	int fdr, fdw;	/* FIFO directories to read and write. */
	char buf[MAX_BUF_SIZE];		/* Buffer to get or print information. */
	char errbuf[MAX_BUF_SIZE];	/* Buffer to print error messages. */
	char clientInfo[5][MAX_NAME_SIZE];	/* Arguments of connected client. */
	char fifoName[MAX_NAME_SIZE];	/* Name for FIFO of the client. */
	char func[MAX_FUNC_SIZE];
	char func2[MAX_FUNC_SIZE];
	char deadpid[MAX_NAME_SIZE];
	FILE* logfile;
	int i=0;
	int end=0;
	long int endtime=0;
	pid_t pid;
	long int t0=0;			/* Connection time of client. */
	long int res=0;			/* Resolution in miliseconds. */
	long int timeInt=0;		/* Time interval in miliseconds. */
	char **endptr;		/* char** for strtol */
	double result=0;
	// timer
	struct timeval tpclient;
	struct timeval tpserver;
	
	while(gettimeofday(&tpserver, NULL));
	
	signal(SIGINT, setDoneFlag);
	
	logfile = fopen("IntegralGen.log", "w");
	
	if(argc != 3) /* argc should be 3 for correct execution */
	{
		printf("\nUsage: %s <resolution> <max # of clients> \n\n", argv[0]);
		return -1;
	}
	if(!argumentsValid(argv))
		return -1;
	maxClient = atoi(argv[2]);
	if(maxClient < 1){
		printf("\nMax number of clients must be greater than 0.\n\n");
		return 0;
	}	
		
	if(mkfifo(SERVER_FIFO, FIFO_PERM) < 0){
		fprintf(stderr, "Could not make fifo file: \"%s\" for server.\n", SERVER_FIFO);
		//return -1;
	}
	
	res = strtol(argv[1], endptr, 10);
	
	fdr = open(SERVER_FIFO, O_RDONLY | O_NONBLOCK);
	fprintf(logfile, "Server is open.\n");
	
	while (!doneFlag) {
		if(readLine(fdr,buf, MAX_BUF_SIZE)>0){
			if(strstr(buf, "--client") != NULL){
				// get connection time of client
				if(gettimeofday(&tpclient, NULL)){
					fprintf(stderr, "Failed to get connection time of client.\n");
					continue;
				}
				strcpy(deadpid,strtok(buf, "--client"));
				t0 = MILLION*(tpclient.tv_sec-tpserver.tv_sec) + tpclient.tv_usec-tpserver.tv_usec;
				// convert t0 to miliseconds
				t0 = t0 / 1000;
				--currentClient;
				fprintf(stderr, "a client died\nClients: %d\n", currentClient);
				fprintf(logfile, "client:%s is terminated at time: %ld\n", deadpid, t0);
			}
			else if(currentClient < maxClient){
				// get connection time of client
				if(gettimeofday(&tpclient, NULL)){
					fprintf(stderr, "Failed to get connection time of client.\n");
					continue;
				}
				t0 = MILLION*(tpclient.tv_sec-tpserver.tv_sec) + tpclient.tv_usec-tpserver.tv_usec;
				// convert t0 to miliseconds
				t0 = t0 / 1000;
				
				//fprintf(stderr,"Connection time: %ld\n", t0);
				// parse arguments
				strcpy(clientInfo[0],strtok(buf, " "));
				++i;
				for(i=1; i<5; ++i) 
					strcpy(clientInfo[i],strtok(NULL, " "));
					
				sprintf(fifoName,"%s.fifo", clientInfo[0]);
				if(mkfifo(fifoName, FIFO_PERM) < 0){
					fprintf(stderr, "Could not make fifo file: \"%s\" for client.\n", fifoName);
				}
				
				fdw = open(fifoName, O_WRONLY);
				if(fdw == -1)
					printf("Couldn't open client fifo\n");
				if(!txtExists(clientInfo[1])){
					sprintf(errbuf, "error: %s is not valid.\n", clientInfo[1]);
					if(write(fdw,errbuf,strlen(errbuf)) == -1){
						fprintf(stderr, "Couldn't write to %s\n", fifoName);
					}
					close(fdw);
					continue;
				}
				if(!txtExists(clientInfo[2])){
					sprintf(errbuf, "error: %s is not valid.\n", clientInfo[2]);
					if(write(fdw,errbuf,strlen(errbuf)) == -1){
						fprintf(stderr, "Couldn't write to %s\n", fifoName);
					}
					close(fdw);
					continue;
				}
				
				++currentClient;
				fprintf(stderr, "Clients: %d\n", currentClient);
				fprintf(logfile, "client:%s is connected at time: %ld\n", clientInfo[0], t0);
				
				readtxt(clientInfo[1], func);
				readtxt(clientInfo[2], func2);
				
				strcat (func, clientInfo[4]);
				strcat (func, func2);
				
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
					close(fdr);
					fclose(logfile);
					
					//fprintf(stderr, "pid: %d func: %s\n", getpid(), func);
					sprintf(buf,"pid:%d\n",getpid());
					if(write(fdw,buf,strlen(buf)) == -1){
						fprintf(stderr, "Couldn't write to %s\n", fifoName);
					}
					
					if(!doneFlag){
						result = 0;
						timeInt = strtol(clientInfo[3], endptr, 10);
						timeInt *= 1000;
						endtime=t0+timeInt;
					}
					while(!doneFlag){
						// calculate result
						result += calculateIntegral(func, t0, t0 + res);
						if ((t0 > endtime) && !end)
						{
							sprintf(buf,"result: %g\n",result);
							// send result
							if(write(fdw,buf,strlen(buf)) == -1){
								fprintf(stderr, "Couldn't write to %s\n", fifoName);
							}
							// inform client that process ended
							if(write(fdw,"end.\n",strlen("end.\n")) == -1){
								fprintf(stderr, "Couldn't write to %s\n", fifoName);
							}
							end = 1;
							fprintf(stderr, "result: %g\n",result);
						}
						t0 = t0 + res;
					}
					
					close(fdw);
					
					// inform server that client is dead
					fdw = open(SERVER_FIFO, O_WRONLY | O_NONBLOCK);
					while(fdw == -1)
						fdw = open(SERVER_FIFO, O_WRONLY | O_NONBLOCK);
					sprintf(buf,"--client%d",getpid());
					if(write(fdw, buf, strlen(buf)) == -1){
						fprintf(stderr, "child couldn't write to %s\n", SERVER_FIFO);
					}
					
					close(fdw);
					exit(0);
				}
				close(fdw);
				
			}else{
				//send client a message that server is full
				strcpy(clientInfo[0],strtok(buf, " "));
				sprintf(fifoName,"%s.fifo", clientInfo[0]);
				if(mkfifo(fifoName, FIFO_PERM) < 0){
					fprintf(stderr, "Could not make fifo file: \"%s\" for client.\n", fifoName);
				}
				fdw = open(fifoName, O_WRONLY);
				sprintf(errbuf, "error: Server is full. Please try again later.\n");
				if(write(fdw, errbuf, strlen(errbuf)) == -1){
					fprintf(stderr, "Couldn't write to %s\n", fifoName);
				}
				close(fdw);
			}
		}
	}
	
	// Wait child processes
	while(pid = wait())
	{
		if((pid == -1) && (errno != EINTR))
			break;
	}
	if(doneFlag)
		fprintf(logfile, "Program is terminated by CTRL + C signal.\n");
	
	fclose(logfile);
	close(fdr);
	unlink(SERVER_FIFO);
	return 0;
}

int argumentsValid(char** arguments){
	int i, result = 1;
	
	for(i=0; i < strlen(arguments[1]); ++i){
		if(arguments[1][i] > '9' || arguments[1][i] < '0'){
			fprintf(stderr, "%s is not a valid number.\n", arguments[1]);
			result = -1;
			break;
		}
	}
	for(i=0; i < strlen(arguments[2]); ++i){
		if(arguments[2][i] > '9' || arguments[2][i] < '0'){
			fprintf(stderr, "%s is not a valid number.\n", arguments[2]);
			result = -1;
			break;
		}
	}
	return result;
}

int txtExists(char* fileName)
{
	FILE* file;
	
	strcat(fileName,".txt");
	file = fopen(fileName, "r");
	
	if ( file == 0 )
		return 0;
	fclose(file);
	return 1;
}

int readLine(int fd, char * line, size_t size) 
{
	char c = 0;
	size_t i = 0;
	if ((NULL != line) && (0 < size))
	{
		while (read(fd, &c, 1) >0) 
		{
			if (('\n' == c) || (size < i)) { 
				break;
			}
			line[i] = c;
			++i;
		}
		line [i] = 0;
	}
	return i;
}

int readtxt(char* fileName, char* buf){
	char c;
	int i=0;
	FILE* file;
	
	file = fopen(fileName, "r");
	if ( file == 0 )
		return 0;

	for (i = 0; i < MAX_FUNC_SIZE; ++i){
		int c = getc(file);
		if (c == EOF){
			buf[i-1] = '\0';
			break;
		}
		buf[i] = c;
	}
	buf[i-1] = '\0';
	
	fclose(file);
	return i;
}

double calculateIntegral(char func[], long t0, long res){
	// Parser variables
	void *f;			/* Evaluator for function. */
	char **names;	/* Function variable names. */
	int count = 1;		/* Number of function variables. */
	double values1[1];		/* Variable t0 value.  */
	double values2[1];		/* Variable t0+res value.  */
	int i;
	double result=0;
	
	values1[0]=t0+res;
	values2[0]=t0;
	
	/* Create evaluator for function.  */
	f = evaluator_create (func);
	if(f == NULL)
		return result;

	/* Find variable names appearing in function. */
	evaluator_get_variables (f, &names, &count);

	/* Calculate and print value of function. */
	result = res * (evaluator_evaluate (f,count,names,values1)-evaluator_evaluate (f,count,names,values2))/(double)2;
	result = fabs(result);

	/* Destroy evaluator.  */
	evaluator_destroy (f);
	return result;
}






