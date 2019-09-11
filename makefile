myshell : main.o
	gcc -o myshell main.c

clean :
	rm *.o myshell.exe
