# CSE244 System Programming HW2

This program asks a word from the user and a directory to search the word in it. Once the user enters the command, program for the “.txt” files in the given directory. If there are child directories, program calls fork to search the given word in the child directories. Found words doesn’t have to be exactly same with search word, program also lists the words which contains the given word. Processes communicate through their own log files. Each main process waits for their child processes.

For example,
 
Given word: ow
Found words: how, low, bow, etc.

Once the program finds the given word, it prints to the command prompt and the log file of the program the information listed below:

-	Column number of the occurrence
-	Line number of the occurrence
-	Full string contains the given word

### Example log content:
```
./test1.txt
 Given word is: ow
 1: "How" line: 0 column: 1
 2: "window?" line: 0 column: 35
 3: "How" line: 2 column: 1
 4: "window?" line: 2 column: 35
 5: "How" line: 10 column: 1
 6: "window?" line: 10 column: 35
 7: "How" line: 12 column: 1
 8: "window?" line: 12 column: 35

./test2.txt
 Given word is: ow
 1: "bowl" line: 7 column: 16
 2: "How" line: 10 column: 1
 3: "window?" line: 10 column: 35
 4: "How" line: 12 column: 1
 5: "window?" line: 12 column: 35

./dir2/test1.txt
 Given word is: ow
 Word not found. 

./dir2/test3.txt
 Given word is: ow
 Word not found. 
```

