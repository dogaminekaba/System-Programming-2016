# CSE244 System Programming HW1


This program asks a word from the user and a “.txt” file name to search the word in it. Once the user enters the command, program checks if the given file name represents a “.txt” file. If it is, program searches for the word inside the given file. Found word doesn’t have to be exactly same with search word, program also lists the words which contains the given word.

For example,
 
Given word: ow
Found words: how, low, bow, etc.

Once the program finds the given word, it prints to the command prompt and the log file of the program the information listed below:

-	Column number of the occurrence
-	Line number of the occurrence
-	Full string contains the given word

### Example log content:
```
test1.txt
 Given word is: ow
 1: "How" line: 0 column: 1
 2: "window?" line: 0 column: 35
 3: "How" line: 2 column: 1
 4: "window?" line: 2 column: 35
 5: "How" line: 10 column: 1
 6: "window?" line: 10 column: 35
 7: "How" line: 12 column: 1
 8: "window?" line: 12 column: 35

test2.txt
 Given word is: aal
 Word not found. 

test2.txt
 Given word is: al
 1: "talks" line: 6 column: 28
 2: "walk" line: 8 column: 32
 3: "sale" line: 13 column: 29

test1.txt
 Given word is: ece
 1: "ecece" line: 3 column: 10
 2: "ecece" line: 3 column: 12
 3: "wecececell" line: 8 column: 16
 4: "wecececell" line: 8 column: 18
 5: "wecececell" line: 8 column: 20
```

