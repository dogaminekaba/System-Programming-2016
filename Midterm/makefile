all:
	gcc -c Client.c
	gcc -o Client Client.o
	gcc -c IntegralGen.c
	gcc IntegralGen.c -I/usr/local/include -L/usr/local/lib -lmatheval -lm -o IntegralGen
