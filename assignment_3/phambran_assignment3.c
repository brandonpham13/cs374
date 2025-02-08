#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define MAX_LINE 1024
#define MAX_FILENAME 256

void create_random_directory(char *dirname) {
    int random_num = rand() % 100000;
    sprintf(dirname, "phambran.movies.%d", random_num);
    
    if (mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP) == 0) {
        printf("Directory created: %s\n", dirname);
    } else {
        perror("Failed to create directory");
        exit(1);
    }
}

// Function to process the CSV file and create text files per year
void process_csv(const char *filename, const char *dirname) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening CSV file");
        exit(1);
    }

    char line[MAX_LINE];
    char title[MAX_LINE];
    int year;
    
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^,],%d", title, &year) == 2) {
            char year_filename[MAX_FILENAME];
            sprintf(year_filename, "%s/%d.txt", dirname, year);
            
            FILE *year_file = fopen(year_filename, "a");
            if (!year_file) {
                perror("Error opening year file");
                continue;
            }
            fprintf(year_file, "%s\n", title);
            fclose(year_file);

            // Set file permissions to rw-r----- (640)
            chmod(year_filename, S_IRUSR | S_IWUSR | S_IRGRP);
        }
    }
    fclose(file);
}

int getValidChoice() {
    char input[10];  // Buffer for user input
    int choice;

    while (1) {
        printf("Enter a choice 1 or 2: ");
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Error reading input.\n");
            exit(EXIT_FAILURE);
        }

        // Check if input is a valid integer between 1 and 2
        if (sscanf(input, "%d", &choice) == 1 && choice >= 1 && choice <= 2) {
            return choice;
        }

        printf("You entered an incorrect choice. Try again.\n\n");
    }
}

// Main function with menu
int main(void) {

    
    int choice, year;
    char language;
    do {
        printf("1. Select file to process\n");
        printf("2. Exit the program\n\n");
        
        choice = getValidChoice();
        
        if (choice == 1) {
            printf("Which file do you want to process?\n");
            printf("Enter 1 to pick the largest file\n");
            printf("Enter 2 to pick the smallest file\n");
            printf("Enter 3 to specify the name of a file\n\n");

            printf("Enter");
        } 
    } while (choice != 2);
    
    // freeMovies(movies);
    return EXIT_SUCCESS;
}