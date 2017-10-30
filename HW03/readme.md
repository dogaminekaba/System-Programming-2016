# CSE244 System Programming HW3 

This program asks a word from the user and a directory to search the word in it. Once the user enters the command, program for the “.txt” files in the given directory. If there are child directories, program calls fork to search the given word in the child directories. Found words doesn’t have to be exactly same with search word, program also lists the words which contains the given word. Processes communicate through pipes and FIFO files. **In this homework, program should have worked parallel, parents weren’t supposed to wait for child processes to collect information and write it to log file but due to my lack of understanding, this program doesn’t work parallel.** It also catches the CTRL + C signal and handles it to avoid possible memory leak.

Example,
 
Given word: ow
Found words: how, low, bow, etc.

Once the program finds the given word, it prints to the command prompt and the log file of the program the information listed below:

-	Column number of the occurrence
-	Line number of the occurrence
-	Full string contains the given word
-	Reason of termination

It also prints the total count of words that found in the files to the terminal, to stderr.

### Example log content:
```
./dmk/asd/System_Programming/boring.txt
 Given word is: o
 Word not found. 

./dmk/asd/System_Programming/test1.txt
 Given word is: o
 1: "How" line: 0 column: 1
 2: "doggie" line: 0 column: 18
 3: "window?" line: 0 column: 35
 4: "one" line: 1 column: 4
 5: "How" line: 2 column: 1
 6: "doggie" line: 2 column: 18
 7: "window?" line: 2 column: 35
 8: "do" line: 3 column: 3
 9: "hope" line: 3 column: 6
 10: "doggie's" line: 3 column: 17
 11: "for" line: 3 column: 26
 12: "to" line: 5 column: 20
 13: "California" line: 5 column: 27
 14: "poor" line: 6 column: 14
 15: "poor" line: 6 column: 15
 16: "alone" line: 6 column: 31
 17: "dog," line: 7 column: 13
 18: "won't" line: 7 column: 21
 19: "lonesome" line: 7 column: 30
 20: "lonesome" line: 7 column: 34
 21: "doggie" line: 8 column: 9
 22: "good" line: 8 column: 34
 23: "good" line: 8 column: 35
 24: "home" line: 8 column: 39
 25: "How" line: 10 column: 1
 26: "doggie" line: 10 column: 18
 27: "window?" line: 10 column: 35
 28: "one" line: 11 column: 4
 29: "How" line: 12 column: 1
 30: "doggie" line: 12 column: 18
 31: "window?" line: 12 column: 35
 32: "do" line: 13 column: 3
 33: "hope" line: 13 column: 6
 34: "doggie's" line: 13 column: 16
 35: "for" line: 13 column: 25

./dmk/asd/System_Programming/test2.txt
 Given word is: o
 1: "robbers" line: 0 column: 32
 2: "love" line: 2 column: 4
 3: "doggie" line: 2 column: 17
 4: "to" line: 2 column: 24
 5: "protect" line: 2 column: 28
 6: "one" line: 3 column: 25
 7: "don't" line: 5 column: 3
 8: "or" line: 5 column: 21
 9: "don't" line: 6 column: 3
 10: "parrot" line: 6 column: 19
 11: "don't" line: 7 column: 3
 12: "bowl" line: 7 column: 16
 13: "of" line: 7 column: 20
 14: "goldfish" line: 8 column: 17
 15: "for" line: 8 column: 26
 16: "How" line: 10 column: 1
 17: "doggie" line: 10 column: 18
 18: "window?" line: 10 column: 35
 19: "one" line: 11 column: 4
 20: "How" line: 12 column: 1
 21: "doggie" line: 12 column: 18
 22: "window?" line: 12 column: 35
 23: "do" line: 13 column: 3
 24: "hope" line: 13 column: 6
 25: "doggie's" line: 13 column: 16
 26: "for" line: 13 column: 25

./dmk/asd/System_Programming/2016/HW02/test1.txt
 Given word is: o
 1: "How" line: 0 column: 1
 2: "doggie" line: 0 column: 18
 3: "window?" line: 0 column: 35
 4: "one" line: 1 column: 4
 5: "How" line: 2 column: 1
 6: "doggie" line: 2 column: 18
 7: "window?" line: 2 column: 35
 8: "do" line: 3 column: 3
 9: "hope" line: 3 column: 6
 10: "doggie's" line: 3 column: 17
 11: "for" line: 3 column: 26
 12: "to" line: 5 column: 20
 13: "California" line: 5 column: 27
 14: "poor" line: 6 column: 14
 15: "poor" line: 6 column: 15
 16: "alone" line: 6 column: 31
 17: "dog," line: 7 column: 13
 18: "won't" line: 7 column: 21
 19: "lonesome" line: 7 column: 30
 20: "lonesome" line: 7 column: 34
 21: "doggie" line: 8 column: 9
 22: "good" line: 8 column: 34
 23: "good" line: 8 column: 35
 24: "home" line: 8 column: 39
 25: "How" line: 10 column: 1
 26: "doggie" line: 10 column: 18
 27: "window?" line: 10 column: 35
 28: "one" line: 11 column: 4
 29: "How" line: 12 column: 1
 30: "doggie" line: 12 column: 18
 31: "window?" line: 12 column: 35
 32: "do" line: 13 column: 3
 33: "hope" line: 13 column: 6
 34: "doggie's" line: 13 column: 16
 35: "for" line: 13 column: 25

./dmk/asd/System_Programming/2016/HW02/test2.txt
 Given word is: o
 1: "robbers" line: 0 column: 32
 2: "love" line: 2 column: 4
 3: "doggie" line: 2 column: 17
 4: "to" line: 2 column: 24
 5: "protect" line: 2 column: 28
 6: "one" line: 3 column: 25
 7: "don't" line: 5 column: 3
 8: "or" line: 5 column: 21
 9: "don't" line: 6 column: 3
 10: "parrot" line: 6 column: 19
 11: "don't" line: 7 column: 3
 12: "bowl" line: 7 column: 16
 13: "of" line: 7 column: 20
 14: "goldfish" line: 8 column: 17
 15: "for" line: 8 column: 26
 16: "How" line: 10 column: 1
 17: "doggie" line: 10 column: 18
 18: "window?" line: 10 column: 35
 19: "one" line: 11 column: 4
 20: "How" line: 12 column: 1
 21: "doggie" line: 12 column: 18
 22: "window?" line: 12 column: 35
 23: "do" line: 13 column: 3
 24: "hope" line: 13 column: 6
 25: "doggie's" line: 13 column: 16
 26: "for" line: 13 column: 25
```

