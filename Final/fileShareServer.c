/*
 *	Doga Mine Kaba
 *	121044004
 *	
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#define MAX_BUF_SIZE 1025
#define MAX_THREAD_SIZE 100 // max client number connects at the same time
#define MAX_NAME_SIZE 1000
#define MILLION 1000000L

int	threadCount = 0;
// timer for server
struct timeval tpserver;

// keeps track of threads
pthread_t tidArr[MAX_THREAD_SIZE];

// keeps track of clients
int connfdArr[MAX_THREAD_SIZE];

// returns the index of first empty place in the tidArr to add new tid
int getEmptyIndex();

void* threadFunc(void *arg);
void sendLocalFileList(int connfd);
void sendClientList(int connfd);
int isDir(char* fileName);
void createFileInServer(char *fileName);

// signal handler
static volatile int doneFlag = 0;
static void setDoneFlag(int signo) {
	doneFlag = 1;
}

// thread parameters
struct thrd_data {
    sem_t * lock;
    int connfd;
};

int main(int argc, char* argv[])
{

	int portNumber = -1;
	int listenfd=0, connfd=0, i=0, index=0;
	struct sockaddr_in server;
	char sendBuff[MAX_BUF_SIZE];
	struct thrd_data data;
	pthread_t tid;
	int err;
	sem_t semlock;
	
	while(gettimeofday(&tpserver, NULL));
	
	signal(SIGINT, setDoneFlag);
	
	if(argc != 2) // argc should be 2 for correct execution
	{
		printf("\nUsage: %s <port number>\n\n", argv[0]);
		return -1;
	}
	
	if(sem_init(&semlock, 0, 1) == -1){
		fprintf(stderr,"Failed to initialize semaphore.\n");
		return -1;
	}
	
	// initialize tidArr to -1
	for(i = 0; i < MAX_THREAD_SIZE; ++i)
		tidArr[i] = -1;
	// initialize connfdArr to -1
	for(i = 0; i < MAX_THREAD_SIZE; ++i)
		connfdArr[i] = -1;
	
	portNumber = atoi(argv[1]);
	printf("Port number is declared to: %d\n", portNumber);
	
	// Create socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM , 0)) < 0)
	{
		fprintf(stderr,"Could not create socket\n");
		return -1;
	}

	// Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( portNumber );
	fcntl(listenfd, F_SETFL, O_NONBLOCK);

	// Bind
	if( bind(listenfd,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		fprintf(stderr,"Socket binding failed.\n");
		return -1;
	}
	fprintf(stderr,"Waiting for connection...\n");

	// Listen
	listen(listenfd, 10); 
	
	memset(&server, '0', sizeof(server));

	// Accept incoming connection
	while(!doneFlag)
	{
		connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
		if(connfd < 0)
			continue;
		if(threadCount < MAX_THREAD_SIZE)
		{
			//data = (struct thrd_data *)malloc(sizeof(struct thrd_data));
			data.connfd = connfd;
			data.lock = &semlock;
			
			
			err = pthread_create(&tid, NULL, &threadFunc, &data);
			if (err != 0)
			{	
				fprintf(stderr,"\ncan't create thread :[%s]", strerror(err));
				sprintf(sendBuff, "An error occured. Try again later.\r\n");
				write(connfd, sendBuff, strlen(sendBuff));
				close(connfd);
			}
			sleep(1);
		}
		else
		{
			fprintf(stderr, "Threads reached maximum count.\n");
			sprintf(sendBuff, "Server is full. Try again later.\r\n");
			write(connfd, sendBuff, strlen(sendBuff));
			close(connfd);
		}
	}
	
	// Join threads
	for(i=0; i<MAX_THREAD_SIZE; ++i)
	{
		if(tidArr[i] != -1)
			if(err = pthread_join(tidArr[i], NULL))
				fprintf(stderr, "Failed to join thread: %s\n", strerror(err));
	}
	
	sem_destroy(&semlock);
	pthread_exit(NULL);
}

int getEmptyIndex()
{
	int i=0;
	for (i = 0; i < MAX_THREAD_SIZE; ++i)
	{
		if(tidArr[i] == -1)
			return i;
	}
	return -1;
}

void* threadFunc(void *arg)
{
	struct thrd_data* data = arg;	

	int connfd = data->connfd;
	char sendBuff[MAX_BUF_SIZE] = {0};
	char recvBuff[MAX_BUF_SIZE-1] = {0};
	sem_t* semlockp = data->lock;
	pthread_t tid = pthread_self();
	int i,n=0;
	int index;
	int clientTerminated = 0;
	struct timeval tpclient;
	long int tStart=0;			/* Connection time of client. */
	long int tReq=0;			/* Request time of client. */
	char *token;
	char destClient[MAX_BUF_SIZE];
	char fileName[MAX_NAME_SIZE];
	char line[MAX_BUF_SIZE];
	FILE *fp;
	int bytesReceived = 0;
	
	//free(data);

	// get connection time of client
	if(gettimeofday(&tpclient, NULL))
		fprintf(stderr, "Failed to get connection time of client.\n");
	
	else
	{
		tStart = MILLION*(tpclient.tv_sec-tpserver.tv_sec) + tpclient.tv_usec-tpserver.tv_usec;
		// convert tStart to miliseconds
		tStart = tStart / 1000;
	}
	fprintf(stderr,"client#%d connected. Connection time: %ld\n", connfd, tStart);
	
	if(sem_wait(semlockp) == -1)
	{
		fprintf(stderr, "Thread failed to lock semaphore.\n");
		pthread_exit(NULL);
	}
	
	++threadCount;
	index = getEmptyIndex();
	if(index==-1)
		clientTerminated=1;

	tidArr[index] = tid;
	connfdArr[index] = connfd;
	fcntl(connfd, F_SETFL, O_NONBLOCK);
	
	if(sem_post(semlockp) == -1)
		fprintf(stderr, "Thread failed to unlock semaphore.\n");
	
	while(!clientTerminated && !doneFlag){
		if((n = read(connfd, recvBuff, sizeof(recvBuff)-1)) > 0)
		{
			recvBuff[n] = '\0';
		
			if(strcmp("listServer",recvBuff)==0){
				if(gettimeofday(&tpclient, NULL))
					fprintf(stderr, "Failed to get connection time of client.\n");
				else
				{
					tReq = MILLION*(tpclient.tv_sec-tpserver.tv_sec) + tpclient.tv_usec-tpserver.tv_usec;
					// convert tReq to miliseconds
					tReq = tReq / 1000;
					fprintf(stderr,"client#%d requested <listServer>. Request time: %ld\n", connfd, tReq);
				}
				sendLocalFileList(connfd);
			}
			if(strcmp("lsClients",recvBuff)==0){
				if(gettimeofday(&tpclient, NULL))
					fprintf(stderr, "Failed to get connection time of client.\n");
				else
				{
					tReq = MILLION*(tpclient.tv_sec-tpserver.tv_sec) + tpclient.tv_usec-tpserver.tv_usec;
					tReq = tReq / 1000;
					fprintf(stderr,"client#%d requested <lsClients>. Request time: %ld\n", connfd, tReq);
				}
				sendClientList(connfd);
			}
			if(strstr(recvBuff, "sendFile") != NULL){
				if(gettimeofday(&tpclient, NULL))
					fprintf(stderr, "Failed to get connection time of client.\n");
				else
				{
					tReq = MILLION*(tpclient.tv_sec-tpserver.tv_sec) + tpclient.tv_usec-tpserver.tv_usec;
					tReq = tReq / 1000;
					fprintf(stderr,"client#%d requested %s Request time: %ld\n", connfd, recvBuff, tReq);
				}
				token = strtok(recvBuff, " ");
				if( token != NULL ) 
				{
					token = strtok(NULL, " ");
					if( token != NULL ) 
					{
						strcpy(fileName, token);
						//fprintf(stderr,"filename: %s -\n", fileName);
						token = strtok(NULL, " ");
						if(token != NULL)
						{
							strcpy(destClient, token);
							// search in current clients
							//fprintf(stderr, "client: %s -\n", destClient);
						}
						else
						{
							fprintf(stderr, "server -\n");
							// create file in server computer
							fp = fopen(fileName, "wb"); 
							if(NULL != fp)
								break;
							
							fcntl(connfd, F_SETFL, ~O_NONBLOCK);
							
							while((bytesReceived = read(connfd, recvBuff, 256)) > 0)
							{
								fprintf(stderr, "bytes: %d -", bytesReceived);
								fwrite(recvBuff, 1, bytesReceived,fp);
							}
							fclose(fp);
							fcntl(connfd, F_SETFL, O_NONBLOCK);
						}
					}
				}
				
			}
			if(strcmp("exit",recvBuff)==0){
				clientTerminated = 1;
			}
		}
	}
	
	// critical section for release
	if(sem_wait(semlockp) == -1)
	{
		fprintf(stderr, "Thread failed to lock semaphore.\n");
		pthread_exit(NULL);
	}
	
	if(doneFlag)
	{
		snprintf(sendBuff, MAX_BUF_SIZE, "exitdmk");
			write(connfd, sendBuff, strlen(sendBuff));
	}
	
	close(connfd);
	if(index!=-1){
		tidArr[index] = -1;
		connfdArr[index] = -1;
	}
	if(clientTerminated)
		fprintf(stderr,"client#%d left. \n", connfd);
	--threadCount;
	
	if(sem_post(semlockp) == -1)
		fprintf(stderr, "Thread failed to unlock semaphore.\n");
		
	pthread_detach(tid);
}

void sendLocalFileList(connfd)
{
	struct dirent* in_file;
	DIR*	FD;
	char sendBuff[MAX_BUF_SIZE] = {0};
	
	if (NULL == (FD = opendir("."))) 
		return;
	
	while ((in_file = readdir(FD)) != NULL) 
	{
		if(!(strcmp(in_file->d_name,".") == 0 || 
			strcmp(in_file->d_name,"..") == 0))
		{
			if (!isDir(in_file->d_name))
			{
				snprintf(sendBuff, MAX_BUF_SIZE,"%s\n" ,in_file->d_name);
				write(connfd, sendBuff, strlen(sendBuff));
			}
		}
	}
	snprintf(sendBuff, MAX_BUF_SIZE,"eofdmk\n");
	write(connfd, sendBuff, strlen(sendBuff));
	closedir(FD);
}

int isDir(char* fileName)
{
	struct stat st;

	if(!stat(fileName, &st) && S_ISDIR(st.st_mode))
		return 1;
	return 0;
}

void sendClientList(connfd)
{
	char sendBuff[MAX_BUF_SIZE] = {0};
	int i;
	
	for(i=0; i<MAX_THREAD_SIZE; ++i)
	{
		//printf("cid: %d\n",connfdArr[i]);
		if(connfdArr[i] > -1)
		{
			snprintf(sendBuff, MAX_BUF_SIZE, "client#%d\n" , connfdArr[i]);
			write(connfd, sendBuff, strlen(sendBuff));
		}
	}
	snprintf(sendBuff, MAX_BUF_SIZE,"eofdmk\n");
	write(connfd, sendBuff, strlen(sendBuff));
}







