/*
 *	Doga Mine Kaba
 *	121044004
 *	
 */

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#define MAX_BUF_SIZE 1000
#define MAX_NAME_SIZE 1000

void displayHelp();
void listLocal();
int readFromServer(int socketd, char recvBuff[], int size);
int isDir(char* fileName);
int fileExists(char* fileName);
int sendFile(char* fileName, int socketd);

// signal handler
static volatile int doneFlag = 0;
static void setDoneFlag(int signo) {
	doneFlag = 1;
}

int main(int argc, char* argv[])
{
	int portNumber = -1, i=0, numOfRead=0, numOfScan=0, serverFlag=0, len=0;
	int socketd = 0;
	char recvBuff[MAX_BUF_SIZE];
	char* ip = NULL;
	struct sockaddr_in serv_addr;
	char command[MAX_BUF_SIZE] = {0};
	char *token;
	char destClient[MAX_BUF_SIZE];
	char fileName[MAX_NAME_SIZE];
	char line[MAX_BUF_SIZE];
	
	signal(SIGINT, setDoneFlag);
	
	// stop scanf and socketd blocking
	fcntl (0, F_SETFL, O_NONBLOCK);
	fcntl (socketd, F_SETFL, O_NONBLOCK);

	if(argc != 2) // argc should be 2 for correct execution
	{
		printf("\nUsage: %s <ip address>\n\n", argv[0]);
		return -1;
	}
	
	// parse ip address and port
	ip = strchr(argv[1], ':');
	if(ip == NULL){
		fprintf(stderr, "Invalid ip address.\n");
		return -1;
	}
	++ip;
	portNumber = atoi(ip);
	printf("Port number is declared to: %d\n", portNumber);
	ip = strtok(argv[1], ":");
	printf("Ip address is declared to: %s\n", ip);
	
	// clear receiver buffer
	memset(recvBuff, '0',sizeof(recvBuff));
	
	// create socket
	if((socketd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		fprintf(stderr, "Could not create socket for communication.\n");
		return -1;
	} 

	// socket configuration
	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portNumber); 

	if(inet_pton(AF_INET, ip, &serv_addr.sin_addr)<=0)
	{
		fprintf(stderr, "inet_pton error occured. Ip address might be invalid.\n");
		return -1;
	} 

	if(connect(socketd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "Connection failed.\n");
		return -1;
	}
	printf("\nConnected to the server.\nYou can enter your command > ");
	
	while(!doneFlag){
		readFromServer(socketd, recvBuff, MAX_BUF_SIZE);
		if(strcmp(recvBuff, "exitdmk") == 0)
		{
			serverFlag = 1;
			break;
		}
		
		if(fgets(command, MAX_BUF_SIZE - 1, stdin) == NULL)
			continue;
		command[strlen(command)-1]='\0';
			
		if(strcmp(command, "help") == 0)
		{
			displayHelp();
			printf("> ");
		}
		else if(strcmp(command, "listLocal") == 0)
		{
			listLocal();
			printf("> ");
		}
		else if(strcmp(command, "listServer") == 0)
		{
			printf("\nServer Files:\n\n");
			write(socketd, command, strlen(command));
			numOfRead = readFromServer(socketd, recvBuff, MAX_BUF_SIZE);
			// -2 means end of transmisson of current message
			while (numOfRead != -2)
			{
				printf("%s", recvBuff);
				numOfRead = readFromServer(socketd, recvBuff, MAX_BUF_SIZE);
			}
			// print last messages stayed in buffer
			if(numOfRead == -2)
				printf("%s", recvBuff);
			printf("\n> ");
		}
		else if (strcmp(command, "lsClients") == 0)
		{
			printf("\nCurrent Clients:\n\n");
			write(socketd, command, strlen(command));
			numOfRead = readFromServer(socketd, recvBuff, MAX_BUF_SIZE);
			// -2 means end of transmisson of current message
			while (numOfRead != -2)
			{
				printf("%s", recvBuff);
				numOfRead = readFromServer(socketd, recvBuff, MAX_BUF_SIZE);
			}
			// print last messages stayed in buffer
			if(numOfRead == -2)
				printf("%s", recvBuff);
			printf("\n> ");
		}
		else if(strstr(command, "sendFile") != NULL)
		{
			strcpy(line, command);
			token = strtok(command, " ");
			if( token != NULL ) 
			{
				token = strtok(NULL, " ");
				
				if( token != NULL ) 
				{
					strcpy(fileName, token);
					if(fileExists(fileName))
					{
						strcpy(command, line);
						write(socketd, command, strlen(command));
						sendFile(fileName, socketd);
					}
					else
						printf("Given filename is not valid.\n");
				}
				else
					printf("Invalid command. Enter \"help\" for information.");
			}
			else
				printf("Invalid command. Enter \"help\" for information.");

			printf("> ");
		}
		else
			printf("Invalid command. Enter \"help\" for information > ");
	}
	if(doneFlag)
	{
		memset(command, '0',sizeof(command));
		sprintf(command, "exit");
		write(socketd, command, strlen(command));
		printf("\nProgram terminated by CTRL^C signal.\n");
	}
	else if (serverFlag)
	{
		printf("\nServer sent CTRL^C signal. Program Terminated.\n");
	}
	return 0;
}

int readFromServer(int socketd, char recvBuff[], int size)
{
	char* search;
	int count = 0;
	fcntl (socketd, F_SETFL, O_NONBLOCK);
	
	if((count = read(socketd, recvBuff, size-1)) > 0)
	{
		recvBuff[count] = '\0';
		search = strstr(recvBuff, "eofdmk");
		if(search != NULL){
			*search = '\0';
			return -2;
		}
		else
			return count;
	}
	else
		recvBuff[0] = '\0';
		return 0;
}

void displayHelp()
{
	printf("\nYou can use these commands:\n\n");
	printf("listLocal\tto list the local files in the directory client program started\n");
	printf("listServer\tto list the files in the current scope of the server-client\n");
	printf("lsClients\tlists the clients currently connected to the server with their respective clientids\n");
	printf("sendFile <filename> <clientid>\tsend the file <filename> ");
	printf("(if file exists) from local directory to the client with client ");
	printf("id clientid. If no client id is given the file is send ");
	printf("to the servers local directory.\n");
	printf("help\tdisplays the available comments and their usage\n\n");
}

void listLocal()
{
	struct dirent* in_file;
	DIR*	FD;
	
	if (NULL == (FD = opendir("."))) 
		return;

	printf("\nLocal Files:\n\n");
	
	while ((in_file = readdir(FD)) != NULL) 
	{
		if(!(strcmp(in_file->d_name,".") == 0 || 
			strcmp(in_file->d_name,"..") == 0))
		{
			if (!isDir(in_file->d_name))
				printf("%s\n", in_file->d_name);
		}
	}
	closedir(FD);
	printf("\n");
	
}

int isDir(char* fileName)
{
	struct stat st;

	if(!stat(fileName, &st) && S_ISDIR(st.st_mode)){
		return 1;
	}
	return 0;
}

int fileExists(char* fileName)
{
	struct dirent* in_file;
	DIR*	FD;
	
	if (NULL == (FD = opendir("."))) 
		return;

	while ((in_file = readdir(FD)) != NULL) 
	{
		if(!(strcmp(in_file->d_name,".") == 0 || 
			strcmp(in_file->d_name,"..") == 0))
		{
			if (!isDir(in_file->d_name))
			{
				if(strcmp(fileName, in_file->d_name) == 0)
					return 1;
			}
		}
	}
	closedir(FD);
	return 0;
}

int sendFile(char* fileName, int socketd)
{
	FILE *fp = fopen(fileName,"rb");
	char buff[256]={0};
	char recvBuff[MAX_BUF_SIZE]={0};
	int nread = 0;
	
	if(fp==NULL)
	{
		printf("File open error");
		return -1;   
	}   
	
	readFromServer(socketd, recvBuff, MAX_BUF_SIZE);
	while(strcmp(recvBuff,"File is sending...\n") != 0)
		readFromServer(socketd, recvBuff, MAX_BUF_SIZE);
	
	printf("%s", recvBuff);
	
	/* First read file in chunks of 256 bytes */
	nread = fread(buff,1,256,fp);
	buff[nread]='\0';

	/* Read data from file and send it */
	while(nread)
	{
		printf("bytes: %d -", nread);
		/* If read was success, send data. */
		write(socketd, buff, nread);
		nread = fread(buff,1,256,fp);
		buff[nread]='\0';
	}
	fclose(fp);
	write(socketd, "eotdmk", strlen("eotdmk"));
	
	while(strcmp(recvBuff,"File is sent.\n") != 0)
		readFromServer(socketd, recvBuff, MAX_BUF_SIZE);
	
	printf("%s", recvBuff);
}








