#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void parse_input(char *input, char **args) {
    int i = 0;
    args[i] = strtok(input, " \t\n");
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " \t\n");
    }
}

void execute_command(char **args) {
    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("Execution failed");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        wait(NULL); // Wait for the child process to finish
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];

    while (1) {
        // Print the prompt
        printf("mysh> ");

        // Read input from the user
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break; // Exit on EOF (Ctrl+D)
        }

        // Parse the input into arguments
        parse_input(input, args);

        // Execute the command
        if (args[0] != NULL) {
            if (strcmp(args[0], "exit") == 0) {
                break; // Exit the shell if the user types "exit"
            }
            execute_command(args);
        }
    }

    return 0;
}