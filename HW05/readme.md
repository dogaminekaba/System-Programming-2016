# CSE244 System Programming HW5


This program asks a word from the user and a directory to search the word in it. Once the user enters the command, program for the “.txt” files in the given directory. If there are child directories, program calls fork to search the given word in the child directories. Found words doesn’t have to be exactly same with search word, program also lists the words which contains the given word. Processes communicate through pipes and FIFO files. Every parent process (processes work in a directory) creates threads to look in txt files, threads share a FIFO file to communicate and a semaphore lock to manage synchronization, in order to work parallel. It also catches the CTRL + C signal and handles it to avoid possible memory leak.

Once the program finds the given word, it prints to the command prompt and the log file of the program the information listed below:

-	Column number of the occurrence
-	Line number of the occurrence
-	Full string contains the given word
-	Reason of termination

It also prints the total count of words that found in the files to the terminal, to stderr.

Example console output:

<p>
  <img align="center" src="https://github.com/dogaminekaba/System-Programming-2016/blob/master/HW05/ss-hw5.png"/>
</p>

Example log content:
```
./readme.txt
Given word is: hi
1: "This" line: 1 column: 1
2: "which" line: 1 column: 38
        
./hw1.txt
 Given word is: hi
Word not found.
        
./vallog.txt
 Given word is: hi
Word not found.
        
./def/nop/dmkdmk.txt
 Given word is: hi
Word not found.
        
./def/nop/boring.txt
 Given word is: hi
Word not found.
        
./def/nop/abc.txt
 Given word is: hi
Word not found.
        
./def/nop/heyyo.txt
 Given word is: hi
 1: "ahiyaa" line: 5 column: 1
        
./def/nop/test1.txt
 Given word is: hi
Word not found.
        
./def/nop/test2.txt
 Given word is: hi
 1: "shine" line: 1 column: 23
 2: "him" line: 2 column: 34
 3: "fishies" line: 7 column: 33
```
