#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>

#define MAX_LINE 1024
#define MAX_FILENAME 256

void create_random_directory(char *dirname) {
    int random_num = rand() % 100000;
    snprintf(dirname, MAX_FILENAME, "phambran.movies.%d", random_num);    
    mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP);
    printf("Created directory with name %s\n", dirname);
}

void process_csv(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("The file %s was not found. Try again.\n", filename);
        return;
    }

    printf("Now processing the chosen file named %s\n", filename);

    char dirname[MAX_FILENAME];
    create_random_directory(dirname);

    char line[MAX_LINE];
    char title[MAX_LINE];
    int year;
    
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^,],%d", title, &year) == 2) {
            char year_filename[MAX_FILENAME];
            snprintf(year_filename, MAX_FILENAME, "%s/%d.txt", dirname, year);            
            
            FILE *year_file = fopen(year_filename, "a");
            if (!year_file) {
                perror("Error opening year file");
                continue;
            }
            fprintf(year_file, "%s\n", title);
            fclose(year_file);
            chmod(year_filename, S_IRUSR | S_IWUSR | S_IRGRP);
        }
    }
    fclose(file);
}

char* find_file(int find_largest) {
    DIR *d = opendir(".");
    struct dirent *dir;
    struct stat file_stat;
    char *selected_file = NULL;
    off_t selected_size = find_largest ? 0 : __INT_MAX__;

    if (!d) {
        perror("Error opening directory");
        exit(1);
    }

    while ((dir = readdir(d)) != NULL) {
        if (strstr(dir->d_name, ".csv") && strncmp(dir->d_name, "movies_", 7) == 0) {
            if (stat(dir->d_name, &file_stat) == 0) {
                if ((find_largest && file_stat.st_size > selected_size) ||
                    (!find_largest && file_stat.st_size < selected_size)) {
                    selected_size = file_stat.st_size;
                    selected_file = strdup(dir->d_name);
                }
            }
        }
    }
    closedir(d);
    return selected_file;
}

int main(void) {
    srand(time(NULL));
    
    int choice;
    do {
        printf("\n1. Select file to process\n");
        printf("2. Exit the program\n\n");
        do {
            printf("Enter a choice 1 or 2: ");
            if (scanf("%d", &choice) != 1 || (choice != 1 && choice != 2)) {
                printf("You entered an incorrect choice. Try again.\n");
                while (getchar() != '\n'); // Clear input buffer
            } else {
                break;
            }
        } while (1);

        if (choice == 1) {
            int file_choice;
            printf("\nWhich file you want to process?\n");
            printf("Enter 1 to pick the largest file\n");
            printf("Enter 2 to pick the smallest file\n");
            printf("Enter 3 to specify the name of a file\n\n");
            do {
                printf("Enter a choice from 1 to 3: ");
                if (scanf("%d", &file_choice) != 1 || file_choice < 1 || file_choice > 3) {
                    printf("You entered an incorrect choice. Try again.\n");
                    while (getchar() != '\n'); // Clear input buffer
                } else {
                    break;
                }
            } while (1);

            char filename[MAX_FILENAME];
            if (file_choice == 1) {
                strcpy(filename, find_file(1));
            } else if (file_choice == 2) {
                strcpy(filename, find_file(0));
            } else if (file_choice == 3) {
                printf("Enter the complete file name: ");
                scanf("%s", filename);
            }

            
            process_csv(filename);
        }
    } while (choice != 2);

    return EXIT_SUCCESS;
}