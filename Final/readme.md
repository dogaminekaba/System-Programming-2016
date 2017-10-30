# CSE244 System Programming Final Report

## fileShareServer.c:

This program performs user requests using TCP/IP over socket programming to connect users to server. Program creates a separate thread per connected users and makes the thread communicate with the user program. The connection time of user and receiving time of user requests are shown as milliseconds. (Starting of the server’s runtime is considered as 0 ms, user connection times are calculated according to this reference point.)

There are 2 commands that user can request from server:
-	listServer: lists the files in the server’s directory.
-	lsClients: lists the user programs which are currently connected to the server.

To list the current users, a clientId is assigned to the users, using their connection directories (connfd). These id numbers are hold in an array. When a user disconnects from the server program, its id in the array is replaced by “-1”. When a new user wants to connect to the server program, it takes the place of first “-1” id in the array. The place of the threads in the tid array that performs the requests of users also assigned the same way the userIds are.  	When a CTRL^C signal occurs in the server system, program handles this signal and sends messages to the users to let them know the server is going to shut down. When users receive that message, ends their programs after releasing taken resources and finishing the remaining jobs. Server program waits for the created threads to join to the main thread and terminates the program.
If a user ends its program before server system, the leaving user’s id and disconnection information is written on the server command prompt. Id of leaving user is updated in the array in order to be useful again. 

#### To run the program, type:
./fileShareServer <port number>

#### Example:
./ fileShareServer 8888

## client.c:

This program connects to fileShareServer program and sends some requests to the server. 
In addition to the requests mentioned above, this program performs 2 more commands: 
-	help: lists the available commands.
-	listLocal: lists the files in the user’s directory.
	
When a CTRL^C signal occurs in the program, signal is handled and an information about termination of the user program is sent to the server. The reason of termination is written on the command prompt and program is terminated. 
#### To run the program, type:
./client <ip address>

#### Example:
./client 127.0.0.1:8888

