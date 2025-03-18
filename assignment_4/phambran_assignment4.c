#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_INPUT_SIZE 2048
#define MAX_ARGS 512

int last_status = 0; // Last foreground process status
int foreground_only = 0; 

void handle_sigint(int sig_num) {
    // Parent ignores SIGINT
    const char msg[] = "\n: ";
    write(STDOUT_FILENO, msg, strlen(msg));
}

void handle_sigtstp(int sig_num) {
    if (foreground_only) {
        const char msg[] = "\nExiting foreground-only mode\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    } else {
        const char msg[] = "\nEntering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    }
    write(STDOUT_FILENO, ": ", 2);
    fflush(stdout);
}

void check_background_processes() {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("background pid %d is done: exit value %d\n", pid, status);
        } else if (WIFSIGNALED(status)) {
            printf("background pid %d is done, terminated by signal %d\n", pid, status);
        }
    }
}

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

    // Tokenize input
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
        if (foreground_only) {
            background = 0;
        }
        
        // Fork process for external command
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            signal(SIGTSTP, SIG_IGN);
            if (background && !foreground_only) {
                signal(SIGINT, SIG_IGN);
                if (!input_file) {
                    int fd = open("/dev/null", O_RDONLY);
                    if (fd == -1) {
                        perror("open /dev/null for input");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
                if (!output_file) {
                    int fd = open("/dev/null", O_WRONLY);
                    if (fd == -1) {
                        perror("open /dev/null for output");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                } else {
                    signal(SIGINT, SIG_DFL); // Foreground process terminates on SIGINT
                }

            }
            // Input redirection
            if (input_file) {
                int fd = open(input_file, O_RDONLY);
                if (fd == -1) {
                    perror("cannot open file for input"); // TODO: place file name in output
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
            perror("execvp"); // TODO: file name here
            exit(1); // Ensure child terminates on failure
        } else if (pid > 0) {
            // Parent process
            if (background && !foreground_only) {
                printf("background pid is %d\n", pid);
            } else {
                while (waitpid(pid, &last_status, 0) > 0);
                if (WIFSIGNALED(last_status)) {
                    printf("terminated by signal: %d\n", WTERMSIG(last_status));
                }
            }
        } else {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    signal(SIGINT, handle_sigint); // Parent process ignores SIGINT
    signal(SIGTSTP, handle_sigtstp); // Parent handles SIGTSTP

    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];
    char *input_file, *output_file;
    int background;

    while (1) {
        check_background_processes();
        write(STDOUT_FILENO, ": ", 2);
        fflush(stdout);
        
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break; // Exit on EOF
        }
        
        parse_input(input, args, &input_file, &output_file, &background);
        execute_command(args, input_file, output_file, background);
    }
    return 0;
}
