# CSE244 System Programming HW4

This program asks a word from the user and a directory to search the word in it. Once the user enters the command, program for the “.txt” files in the given directory. If there are child directories, program calls fork to search the given word in the child directories. Found words doesn’t have to be exactly same with search word, program also lists the words which contains the given word. Processes communicate through pipes and FIFO files. Every parent process (processes work in a directory) creates threads to look in txt files, threads share a FIFO file to communicate and a single mutex lock to manage synchronization, in order to work parallel. It also catches the CTRL + C signal and handles it to avoid possible memory leak.

Once the program finds the given word, it prints to the command prompt and the log file of the program the information listed below:

-	Column number of the occurrence
-	Line number of the occurrence
-	Full string contains the given word
-	Reason of termination

It also prints the total count of words that found in the files to the terminal, to stderr.

Example console output:

<p>
  <img align="center" src="https://github.com/dogaminekaba/System-Programming-2016/blob/master/HW04-Mutex/ss-hw4.png"/>
</p>

Example log content:
```
./test1.txt
 Given word is: i
 1: "is" line: 0 column: 9
 2: "doggie" line: 0 column: 21
 3: "in" line: 0 column: 24
 4: "window?" line: 0 column: 32
 5: "with" line: 1 column: 9
 6: "tail" line: 1 column: 26
 7: "is" line: 2 column: 9
 8: "doggie" line: 2 column: 21
 9: "in" line: 2 column: 24
 10: "window?" line: 2 column: 32
 11: "doggie's" line: 3 column: 20
 12: "trip" line: 5 column: 16
 13: "California" line: 5 column: 25
 14: "California" line: 5 column: 30
 15: "doggie" line: 8 column: 12
 16: "is" line: 10 column: 9
 17: "doggie" line: 10 column: 21
 18: "in" line: 10 column: 24
 19: "window?" line: 10 column: 32
 20: "with" line: 11 column: 9
 21: "tail" line: 11 column: 26
 22: "is" line: 12 column: 9
 23: "doggie" line: 12 column: 21
 24: "in" line: 12 column: 24
 25: "window?" line: 12 column: 32
 26: "doggie's" line: 13 column: 19
         
./test2.txt
 Given word is: i
 1: "in" line: 0 column: 7
 2: "With" line: 1 column: 1
 3: "flashlights" line: 1 column: 11
 4: "shine" line: 1 column: 24
 5: "in" line: 1 column: 28
 6: "doggie" line: 2 column: 20
 7: "him" line: 2 column: 35
 8: "with" line: 3 column: 21
 9: "kitty" line: 5 column: 27
 10: "little" line: 7 column: 24
 11: "fishies" line: 7 column: 31
 12: "fishies" line: 7 column: 34
 13: "goldfish" line: 8 column: 21
 14: "is" line: 10 column: 9
 15: "doggie" line: 10 column: 21
 16: "in" line: 10 column: 24
 17: "window?" line: 10 column: 32
 18: "with" line: 11 column: 9
 19: "tail" line: 11 column: 26
 20: "is" line: 12 column: 9
 21: "doggie" line: 12 column: 21
 22: "in" line: 12 column: 24
 23: "window?" line: 12 column: 32
 24: "doggie's" line: 13 column: 19
 
 
 Program is terminated by CTRL + C signal. 
 ```
 
 
 
