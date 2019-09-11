// Peter deNoyelles
// 09/26/2018
// CSCI 3453 Lab 1

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>

#define WRITE_END 1
#define READ_END 0

void errorPrint(char *command){
    fprintf(stderr, "Command not found\nFailed to execute '%s'\n", command);
}


//This function trims leading and trailing space characters.
void trim(char *input) {
    if (input == NULL)
        return;

    char *inputCopy1 = input;
    char *inputCopy2 = input;
    char *endChar;

    //Trim leading spaced by simply ignoring them
    while (isspace((unsigned char) *inputCopy2)) {
        ++inputCopy2;
    }

    //Trim trailing spaces
    endChar = inputCopy2 + strlen(inputCopy2) - 1;
    while (endChar > inputCopy2 && isspace((unsigned char) *endChar)) {
        *endChar-- = 0;
    }

    if (inputCopy2 != inputCopy1) {
        while ((*inputCopy1++ = *inputCopy2++));
    }
}

//This function executes a change directory command
void changeDirExecute(char *firstCommand, char *firstArg) {
    pid_t pid;
    int fd[2];
    char directory[1024];

    pipe(fd);
    pid = fork();

    if (pid == 0) {
        //First Command Child Process
        getcwd(directory, sizeof(directory));
        printf("Current working directory is: %s\n", directory);
        chdir(firstArg);
        getcwd(directory, sizeof(directory));
        printf("Path changed to: %s\n", directory);
        exit(1);
    } else {
        //Parent Process
        int status;
        waitpid(pid, &status, 0);
        getcwd(directory, sizeof(directory));
        printf("Exited Child Process...\nPath reverted to %s\n", directory);
    }
}

//This function executes any single command with an argument
void oneProcessExecute(char *firstCommand, char *firstArg) {
    int pid;
    pid = fork();

    if (pid == 0) {
        //Child Process
        execlp(firstCommand, firstCommand, firstArg, (char *) NULL);
        errorPrint(firstCommand);
        exit(0);
    }
    else {
        //Parent Process
        int status;
        waitpid(pid, &status, 0);
    }
}

//This function executes and single command with a file argument
void oneFileInProcessExecute(char *firstCommand, char *firstArg, char *fileName) {
    int pid;
    int fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    pid = fork();

    if (pid == 0) {
        //Child Process
        dup2(fd,1);
        close(fd);
        if (firstArg == NULL)
            execlp(firstCommand, firstCommand, fileName, (char *) NULL);
        else
            execlp(firstCommand, firstCommand, firstArg, fileName, (char *) NULL);
        errorPrint(firstCommand);
        exit(0);
    }
    else {
        //Parent Process
        int status;
        close(fd);
        waitpid(pid, &status, 0);
    }
}

void oneFileOutProcessExecute(char *firstCommand, char *firstArg, char *fileName) {
    int pid;
    int fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    pid = fork();

    if (pid == 0) {
        //Child Process
        dup2(fd,1);
        close(fd);
        execlp(firstCommand, firstCommand, firstArg, (char *) NULL);
        errorPrint(firstCommand);
        exit(0);
    }
    else {
        //Parent Process
        int status;
        close(fd);
        waitpid(pid, &status, 0);
    }
}

//This function executes two commands/arguments seperated by a pipe delimter
void twoProcessExecute(char *firstCommand, char *firstArg, char *secCommand, char *secArg) {
    pid_t pid;
    int fd[2];

    pipe(fd);
    pid = fork();

    if (pid == 0) {
        //First Command Child Process
        dup2(fd[WRITE_END], STDOUT_FILENO);
        close(fd[READ_END]);
        close(fd[WRITE_END]);
        execlp(firstCommand, firstCommand, firstArg, (char *) NULL);
        errorPrint(firstCommand);
        exit(1);
    } else {
        pid = fork();

        if (pid == 0) {
            //Second Command Child Process
            dup2(fd[READ_END], STDIN_FILENO);
            close(fd[WRITE_END]);
            close(fd[READ_END]);
            execlp(secCommand, secCommand, secArg, (char *) NULL);
            errorPrint(firstCommand);
            exit(1);
        } else {
            //Parent Process
            int status;
            close(fd[READ_END]);
            close(fd[WRITE_END]);
            waitpid(pid, &status, 0);
        }
    }
}

int runShell(){
    char command[100];
    char *commandCopy;
    char *firstStr = NULL;
    char *firstCommand = NULL;
    char *firstArg = NULL;
    char *secStr = NULL;
    char *secCommand = NULL;
    char *secArg = NULL;
    char *outPosition = NULL;
    char *inPosition = NULL;
    char *token;
    int i;

    printf("%s", "myshell>");
    scanf("%[^\n]%*c", command);
    commandCopy = strdup(command);
    printf("%s\n", command);

    outPosition = strchr(command, '>');
    inPosition = strchr(command, '<');

    //If "exit", stop the program.
    if (strcmp(command, "exit") == 0)
        exit(0);

    //Separate Input by Pipe characters;
    if (strchr(commandCopy, '|') != 0){
        i = 0;
        while ((token = strsep(&commandCopy, "|")) != NULL) {
            if (i == 0){
                firstStr = token;
                trim(firstStr);
            }
            else {
                secStr = token;
                trim(secStr);
            }
            i++;
        }
    }
        //Separate strings by output character
    else if (strchr(commandCopy, '>') != 0){
        i = 0;
        while ((token = strsep(&commandCopy, ">")) != NULL) {
            if (i == 0){
                firstStr = token;
                trim(firstStr);
            }
            else {
                secStr = token;
                trim(secStr);
            }
            i++;
        }
    }
        //Separate strings by input character
    else if (strchr(commandCopy, '<') != 0){
        i = 0;
        while ((token = strsep(&commandCopy, "<")) != NULL) {
            if (i == 0){
                firstStr = token;
                trim(firstStr);
            }
            else {
                secStr = token;
                trim(secStr);
            }
            i++;
        }
    }

    //Separate firstString by spaces
    if (firstStr == NULL) {
        i = 0;
        while ((token = strsep(&commandCopy, " ")) != NULL) {
            if (i == 0) {
                firstCommand = token;
                trim(firstCommand);
            }
            else {
                firstArg = token;
                trim(firstArg);
            }
            i++;
        }
    } else {
        i = 0;
        while ((token = strsep(&firstStr, " ")) != NULL) {
            if (i == 0) {
                firstCommand = token;
                trim(firstCommand);
            }
            else {
                firstArg = token;
                trim(firstArg);
            }
            i++;
        }
    }


    //Separate secString by spaces (if it exists)
    if (secStr != NULL) {
        i = 0;
        while ((token = strsep(&secStr, " ")) != NULL) {
            if (i == 0){
                secCommand = token;
                trim(secCommand);
            }
            else {
                secArg = token;
                trim(secArg);
            }
            i++;
        }
    }


    //Determine which process executor to run
    if (outPosition != NULL)
        oneFileOutProcessExecute(firstCommand, firstArg, secCommand);
    else if (inPosition != NULL)
        oneFileInProcessExecute(firstCommand, firstArg, secCommand);
    else if (secCommand != NULL)
        twoProcessExecute(firstCommand, firstArg, secCommand, secArg);
    else if (strcmp(firstCommand, "cd") == 0)
        changeDirExecute(firstCommand, firstArg);
    else
        oneProcessExecute(firstCommand, firstArg);
}

int main() {
    //Continuously run shell program
    while (1)
        runShell();
}
