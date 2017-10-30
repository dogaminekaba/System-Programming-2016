
This program works for certain sizes which are defined as macros.
txt input must contain less characters than MAX_RESULT_BUF_SIZE.
I haven't use any dynamic memory allocation so I didn't cause any definately lost leaks.
Program has reachable leaks because of __fopen__.
