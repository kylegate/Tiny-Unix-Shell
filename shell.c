#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

int MAX_LENGTH = 1024;

char *trim(char *str);
void InteractiveMode();
void BatchMode(FILE *BatchFile);
int ExecuteCommand(char *command, char *args[]);

// 1 arg interactive, 2 arg batch, neither incorrect input
int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("<|-_-|> ");
        InteractiveMode();
    }
    if (argc > 2){
        perror("Incorrect number of command line arguments");
        return EXIT_FAILURE;
    }
    else {
        FILE *BatchFile = fopen(argv[1], "r"); // open batch file in read mode
        if (BatchFile == NULL) {
            perror("Error opening batch file");
        }
        BatchMode(BatchFile);
        fclose(BatchFile);
    }
}

// redo function
char* trim(char* str) {
    if (str == NULL || *str == '\0') {return str;}
    while (isspace((unsigned char)(*str))) {str++;} // leading whitespaces
    if (*str == '\0') {return str;} // if string now NULL
    char* end = str + strlen(str) - 1; // trailing whitespaces
    while (end > str && isspace((unsigned char)(*end))) {end--;}
    *(end + 1) = '\0'; // add null terminator
    return str;
}

void InteractiveMode() {
    char input[MAX_LENGTH]; // user input storage
    char delim[] = " \t\r\n"; // new line chars
    int quitcommand = 0; // flag to detect quit command
    char *argument, *command;

    while (1) {
        fflush(stdout);
        int ArgCount = 0;
        int CommandCount = 0;
        char *ptr1, *ptr2, *arguments; // ptrs to use for strtok_r()
        char *commands[MAX_LENGTH]; // storage for array of line commands
        char *args[MAX_LENGTH]; // storage for array of arguments

        if (fgets(input, MAX_LENGTH, stdin) == NULL) { break; }
        /// break commands into individual commands seperated by ;
        command = strtok_r(input, ";", &ptr1);
        while (command != NULL) {
            command = trim(command);
            if (command == NULL || *command == '\0') { // NULL commands / Empty commands
                command = strtok_r(NULL, ";", &ptr1);
                continue;
            }
            if (strcmp(command, "quit") == 0) { // check for quit
                quitcommand = 1;
                command = strtok_r(NULL,";",&ptr1);
                continue;
            }
            else {commands[CommandCount++] = command;} // add to commands array
            command = strtok_r(NULL,";",&ptr1);
        }
        /// arguments added to arguments array
        for (int i = 0 ; i < CommandCount ; i++) {
            ArgCount = 0;
            arguments = commands[i];
            argument = strtok_r(arguments, delim, &ptr2);
            while (argument != NULL) {
                args[ArgCount++] = argument; // store into the arguments array
                argument = strtok_r(NULL, delim, &ptr2);
            }
            // arguments are executed
            if (ArgCount > 0) {
                args[ArgCount] = NULL; // null terminator
                int EStatus = ExecuteCommand(args[0], args);
                if (EStatus != 0) {printf("command does not exist or cannot be executed\n");}
            }
        }
        if (quitcommand != 0) { exit(0); }
        printf("<|-_-|> ");
    }
}

// execution of commands creating forks
int ExecuteCommand(char *command, char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failure");
        return 1;
    }
    else if (pid == 0) {
        execvp(command, args);
        // error occurred
        perror("execvp error");
        fprintf(stderr, "Failure - Command: %s\n", command);
        exit(EXIT_FAILURE);
    }
}`

// Mode read file and interpret commands
void BatchMode(FILE *BatchFile){
    char input[MAX_LENGTH]; // user input storage
    char delim[] = " \t\r\n"; // new line chars
    int quitcommand = 0; // flag to detect quit command
    char *argument, *command;
    while (fgets(input, MAX_LENGTH, BatchFile) != NULL) {
        printf("%s", input); // print command to shell
        int ArgCount = 0;
        int CommandCount = 0;
        char *ptr1, *ptr2, *arguments;
        char *commands[MAX_LENGTH]; // storage of line commands
        char *args[MAX_LENGTH]; // storage for arguments
        /// break commands into individual commands seperated by ;
        command = strtok_r(input, ";", &ptr1);
        while (command != NULL) {
            command = trim(command);
            if (command == NULL || *command == '\0') { // NULL commands / Empty commands
                command = strtok_r(NULL, ";", &ptr1);
                continue;
            }
            if (strcmp(command, "quit") == 0) { // check for quit
                quitcommand = 1;
                command = strtok_r(NULL,";",&ptr1);
                continue;
            }
            else {
                commands[CommandCount++] = command;
            }
            command = strtok_r(NULL,";",&ptr1);
        }    // commands array full from line
        /// arguments added to arguments array
        for (int i = 0 ; i < CommandCount ; i++) {
            ArgCount = 0;
            arguments = commands[i];
            argument = strtok_r(arguments, delim, &ptr2);
            while (argument != NULL) {
                args[ArgCount++] = argument; // store into the arguments array
                argument = strtok_r(NULL, delim, &ptr2);
            }
            // execute arguments
            if (ArgCount > 0) {
                args[ArgCount] = NULL; // null terminator
                int EStatus = ExecuteCommand(args[0], args);
                if (EStatus != 0) {
                    printf("command does not exist or cannot be executed\n");
                }
            }
        }
        if (quitcommand != 0) { exit(0); } // check quit flag
    }
}