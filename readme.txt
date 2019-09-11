*******************************************************
*  Name      :  Peter deNoyelles        
*  Student ID:  830316573               
*  Class     :  CSC 3453
*  Lab#      :  1              
*  Due Date  :  Sep. 26, 2018
*******************************************************


                 Read Me


*******************************************************
*  Description of the program
*******************************************************

This program mimics a linux shell. It takes user input, forks a parent and child process, then uses the child process to execute the given command. The shell program prints out the command entered by the user and then the results of the command. You can exit the program by typing "exit" into the shell.

*******************************************************
*  Source files
*******************************************************

Name: main.cpp
    Main program
	This is the only file in this program. It contains the main driver function as well as all of the auxiliary functions that execute the given shell commands
	
*******************************************************
*  How to build and run the program
*******************************************************

1. Uncompress the .tar file

2. Build the program.
	Change into the directory that contains the file:
	% cd deN6573
	
	Compile the program:
	% make
	
3. Run the program
	% ./myshell
	
4. Delete the object and executable files:
	# make clean