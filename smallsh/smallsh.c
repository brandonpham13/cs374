#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 2048
#define MAX_ARGS 512

int last_status = 0;

void parse_input(char *input, char **args, char **input_file, char **output_file, int *background) {
    int i = 0;
    *input_file = NULL;
    *output_file = NULL;
    *background = 0;

    // Ignore comment lines
    if (input[0] == '#') {
        args[0] = NULL;
        return;
    }

    // Tokenize the input
    args[i] = strtok(input, " \t\n");
    while (args[i] != NULL) {
        if (strcmp(args[i], "<") == 0) {
            // Input redirection
            args[i] = NULL; // End of arguments before input file
            *input_file = strtok(NULL, " \t\n");
        } else if (strcmp(args[i], ">") == 0) {
            // Output redirection
            args[i] = NULL; // End of arguments before output file
            *output_file = strtok(NULL, " \t\n");
        } else {
            i++;
        }
        args[i] = strtok(NULL, " \t\n");
    }

    if (i > 0 && strcmp(args[i - 1], "&") == 0) {
        *background = 1; // Enable background execution
        args[i - 1] = NULL; // Remove & from arguments
    }
}

void execute_command(char **args, char *input_file, char *output_file, int background) {
    if (args[0] == NULL) {
        return; // No command entered
    }
    
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL) {
            char *home = getenv("HOME");
            if (home) {
                chdir(home);
            } else {
                fprintf(stderr, "ch: HOME env variable not set\n");
            }
        } else {
            if (chdir(args[1]) != 0) {
                perror("cd");
            }
        }
    } else if (strcmp(args[0], "status") == 0) {
        if (WIFEXITED(last_status)) {
            printf("exit value %d\n", WEXITSTATUS(last_status));
        } else if (WIFSIGNALED(last_status)) {
            printf("terminated by signal %d\n", WTERMSIG(last_status));
        }
    } else {
        // Fork process for external command
        pid_t pid = fork();
        if (pid == 0) {
            
            // Child process

            // Input redirection
            if (input_file) {
                int fd = open(input_file, O_RDONLY);
                if (fd == -1) {
                    perror("open input file failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDIN_FILENO); 
                close(fd);
            }
            
            // Output redirection
            if (output_file) {
                int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) {
                    perror("open output file failed");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            execvp(args[0], args);
            perror("execvp"); // If execvp fails, print error
            exit(1); // Ensure child terminates on failure
        } else if (pid > 0) {
            // Parent process
            if (background) {
                printf("background pid is %d\n", pid);
            } else {
                waitpid(pid, &last_status, 0);
            }
        } else {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];
    char *input_file, *output_file;
    int background;

    while (1) {
        printf(": ");
        fflush(stdout);
        
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break; // Exit on EOF
        }
        
        parse_input(input, args, &input_file, &output_file, &background);
        execute_command(args, input_file, output_file, background);
    }
    return 0;
}