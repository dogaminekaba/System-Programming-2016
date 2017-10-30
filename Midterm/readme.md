# CSE244 System Programming Midterm



## IntegralGen.c

This program calculates the results of given integrals within the given boundaries which are the requests of the clients that connects to the program.

Firstly, server creates a FIFO file to wait for the clients. Clients open this file and sends the file names of two distinct functions to calculate the integral from, indexes, time interval, operator between those two functions and client Pid (process id). Server gets that information, reads the functions from given filenames, adds the operator between given functions, opens another FIFO which has the same name with client Pid and calls fork. The child process calculates integral within given boundaries using numeric integral calculation and sends the result to the server with client’s FIFO file. 

The parser in this program belongs to “matheval” library and to make it work, it has to be installed before compiling the program, using “sudo apt-get install libmatheval-dev” command.

After fork call, parent returns to read the server FIFO and waits for another client. It takes clients up to the given maximum client number which is given as an argument to the program. If program reaches maximum client number, it sends the clients that the server is busy.

In the created log file, connection and disconnection times of clients are printed.

#### To start the program:
./integralGen <resolution> <max client number>

#### Example:
./integralGen 100 2

## Client.c

This program connects to integralGen program and requests the calculation of the specified ingetrals with a defined time interval. Connects to the FIFO of server and sends the required arguments and its own Pid. If server is not running at the moment, client program waits until server starts to run. After client is connected to the server, on another FIFO created by server for client, client waits for the result of the calculation. It prints the result and the cause of termination to its log file.

#### To start the program:
./Client <function file 1> <function file 2> <time interval seconds> <operator>

#### Example:
./Client f1 f2 10 +




### Example server log content:
```
Server is open.
client:3285 is connected at time: 3488
Server is open.
client:3285 is connected at time: 3488
client:3284 is connected at time: 3488
Server is open.
client:3285 is connected at time: 3488
client:3284 is connected at time: 3488
client:3283 is connected at time: 3493
client:3282 is connected at time: 3501
Server is open.
client:3285 is connected at time: 3488
client:3284 is connected at time: 3488
client:3283 is connected at time: 3493
client:3282 is connected at time: 3501
client:3286 is connected at time: 3501
Server is open.
client:3285 is connected at time: 3488
client:3284 is connected at time: 3488
client:3283 is connected at time: 3493
Server is open.
client:3285 is connected at time: 3488
client:3284 is connected at time: 3488
client:3283 is connected at time: 3493
client:3282 is connected at time: 3501
client:3286 is connected at time: 3501
client:3288 is terminated at time: 5505
client:3292 is terminated at time: 5507
client:3291 is terminated at time: 5508
client:3290 is terminated at time: 5508
client:3289 is terminated at time: 7507
client:3294 is connected at time: 13403
Server is open.
client:3285 is connected at time: 3488
client:3284 is connected at time: 3488
client:3283 is connected at time: 3493
client:3282 is connected at time: 3501
client:3286 is connected at time: 3501
client:3288 is terminated at time: 5505
client:3292 is terminated at time: 5507
client:3291 is terminated at time: 5508
client:3290 is terminated at time: 5508
client:3289 is terminated at time: 7507
client:3294 is connected at time: 13403
client:3293 is connected at time: 13403
client:3295 is connected at time: 13403
Server is open.
client:3285 is connected at time: 3488
client:3284 is connected at time: 3488
client:3283 is connected at time: 3493
client:3282 is connected at time: 3501
client:3286 is connected at time: 3501
client:3288 is terminated at time: 5505
client:3292 is terminated at time: 5507
client:3291 is terminated at time: 5508
client:3290 is terminated at time: 5508
client:3289 is terminated at time: 7507
client:3294 is connected at time: 13403
client:3293 is connected at time: 13403
Server is open.
client:3285 is connected at time: 3488
client:3284 is connected at time: 3488
client:3283 is connected at time: 3493
client:3282 is connected at time: 3501
client:3286 is connected at time: 3501
client:3288 is terminated at time: 5505
client:3292 is terminated at time: 5507
client:3291 is terminated at time: 5508
client:3290 is terminated at time: 5508
client:3289 is terminated at time: 7507
client:3294 is connected at time: 13403
client:3293 is connected at time: 13403
client:3295 is connected at time: 13403
client:3296 is connected at time: 13409
```
