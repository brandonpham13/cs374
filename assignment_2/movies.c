#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LANGUAGES 5
#define MAX_LANG_LENGTH 20

// Define movie struct
typedef struct movie {
    char *title;
    int year;
    char languages[MAX_LANGUAGES][MAX_LANG_LENGTH];
    int num_languages;
    float rating;
    struct movie *next;
} Movie;

Movie* createMovie(char *line) {
    Movie *newMovie = malloc(sizeof(Movie));

    // Make a copy of the input line to avoid modifying the original
    char *line_copy = strdup(line);
    char *temp = line_copy;  // Store original pointer for free() later

    // Tokenize by commas
    char *tokens[4]; 
    int i = 0;
    char *token = strtok(line_copy, ",");
    while (token != NULL && i < 4) {
        tokens[i++] = token;
        token = strtok(NULL, ",");
    }

    // Process title
    newMovie->title = malloc(strlen(tokens[0]) + 1);
    strcpy(newMovie->title, tokens[0]);

    // Process year
    newMovie->year = atoi(tokens[1]);

    // Process languages
    char lang_copy[256];
    strcpy(lang_copy, tokens[2]);  // Copy to avoid modifying the original token
    memmove(lang_copy, lang_copy + 1, strlen(lang_copy)); // Remove opening bracket
    lang_copy[strlen(lang_copy) - 1] = '\0'; // Remove closing bracket

    newMovie->num_languages = 0;
    char *lang = strtok(lang_copy, ";");
    while (lang != NULL && newMovie->num_languages < MAX_LANGUAGES) {
        strcpy(newMovie->languages[newMovie->num_languages], lang);
        newMovie->num_languages++;
        lang = strtok(NULL, ";");
    }

    // Process rating
    newMovie->rating = atof(tokens[3]);

    newMovie->next = NULL;

    // Free the duplicated line copy
    free(temp);

    return newMovie;
}


// // Function to create a new movie node
// Movie* createMovie(char *line) {
//     Movie *newMovie = malloc(sizeof(Movie));
//     char *token;
    
//     // Title
//     token = strtok(line, ",");
//     newMovie->title = malloc(strlen(token) + 1);
//     strcpy(newMovie->title, token);

//     // Year
//     token = strtok(NULL, ",");
//     printf("line at year: %s\n", line); //
//     printf("year token: %s\n", token); //

//     newMovie->year = atoi(token);

//     // Languages (Parsing format [English;Spanish])
//     token = strtok(NULL, ",");

//     char lang_copy[256];
//     strcpy(lang_copy, token); // copy bracketed token to new var

//     printf("lang token: %s\n", lang_copy); //

//     memmove(lang_copy, lang_copy + 1, strlen(lang_copy)); // Skip opening bracket
//     lang_copy[strlen(lang_copy) - 1] = '\0'; // Remove closing bracket if present
//     printf("lang token after bracket removal: %s\n", lang_copy); //

//     newMovie->num_languages = 0;
//     char *lang = strtok(lang_copy, ";");
//     while (lang != NULL && newMovie->num_languages < MAX_LANGUAGES) {
//         strcpy(newMovie->languages[newMovie->num_languages], lang);
//         newMovie->num_languages++;
//         lang = strtok(NULL, ";");
//     }

//     printf("token 1: %s\n", token); //

//     // Rating
//     token = strtok(NULL, ",");

//     printf("token 2: %s\n", token); //
//     if (token) {
//         newMovie->rating = atof(token);
//     } else {
//         newMovie->rating = 0.0; // Default if missing
//     }

//     newMovie->next = NULL;
//     printf("Parsed Movie: %s, Year: %d, Rating: %.1f\n", newMovie->title, newMovie->year, newMovie->rating);

//     return newMovie;
// }


// Function to append movie to linked list
Movie* appendMovie(Movie *head, Movie *newMovie) {
    if (head == NULL) return newMovie;
    Movie *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newMovie;
    return head;
}

// Function to read the CSV file and create linked list
Movie* readMoviesFile(char *filePath, int *movieCount) {
    FILE *file = fopen(filePath, "r");
    if (!file) {
        printf("Error opening file!\n");
        exit(EXIT_FAILURE);
    }
    
    char line[1024];
    Movie *head = NULL;
    *movieCount = 0;
    
    fgets(line, sizeof(line), file); // Skip header line
    while (fgets(line, sizeof(line), file)) {
        (*movieCount)++;
        line[strcspn(line, "\n")] = 0; // Remove trailing newline
        Movie *newMovie = createMovie(line);
        head = appendMovie(head, newMovie);
    }
    fclose(file);
    return head;
}

// Function to print movies of a specific year
void printMoviesByYear(Movie *head, int year) {
    Movie *temp = head;
    int found = 0;
    while (temp != NULL) {
        if (temp->year == year) {
            printf("%s\n", temp->title);
            found = 1;
        }
        temp = temp->next;
    }
    if (!found) printf("No data about movies released in the year %d\n", year);
}

// Function to find highest rated movie for each year
void highestRatedMovies(Movie *head) {
    for (int year = 1900; year <= 2021; year++) {
        Movie *temp = head;
        Movie *bestMovie = NULL;
        while (temp != NULL) {
            if (temp->year == year && (bestMovie == NULL || temp->rating > bestMovie->rating)) {
                bestMovie = temp;
            }
            temp = temp->next;
        }
        if (bestMovie) {
            printf("%d %.1f %s\n", year, bestMovie->rating, bestMovie->title);
        }
    }
    printf("\n");
}


// Function to print movies available in a specific language
void printMoviesByLanguage(Movie *head, char *language) {
    Movie *temp = head;
    int found = 0;
    while (temp != NULL) {
        for (int i = 0; i < temp->num_languages; i++) {
            if (strcasecmp(temp->languages[i], language) == 0) {
                printf("%d %s\n", temp->year, temp->title);
                found = 1;
                break;
            }
        }
        temp = temp->next;
    }
    if (!found) printf("No data about movies released in %s\n", language);
}

// Function to free memory
void freeMovies(Movie *head) {
    Movie *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->title);
        free(temp);
    }
}

int getValidChoice() {
    char input[10];  // Buffer for user input
    int choice;

    while (1) {
        printf("Enter a choice from 1 to 4: ");
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Error reading input.\n");
            exit(EXIT_FAILURE);
        }

        // Check if input is a valid integer between 1 and 4
        if (sscanf(input, "%d", &choice) == 1 && choice >= 1 && choice <= 4) {
            return choice;
        }

        printf("You entered an incorrect choice. Try again.\n\n");
    }
}

// Main function with menu
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <movies_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int movieCount;
    Movie *movies = readMoviesFile(argv[1], &movieCount);
    printf("Processed file %s and parsed data for %d movies\n\n", argv[1], movieCount);
    
    int choice, year;
    char language[MAX_LANG_LENGTH];
    do {
        printf("1. Show movies released in the specified year\n");
        printf("2. Show highest rated movie for each year\n");
        printf("3. Show the title and year of release of all movies in a specific language\n");
        printf("4. Exit from the program\n\n");
        
        choice = getValidChoice();
        
        switch (choice) {
            case 1:
                printf("Enter a year for which you want to see movies: ");
                scanf("%d", &year);
                printMoviesByYear(movies, year);
                printf("\n");
                break;
            case 2:
                highestRatedMovies(movies);
                break;
            case 3:
                printf("Enter the language for which you want to see movies: ");
                scanf("%s", language);
                printMoviesByLanguage(movies, language);
                printf("\n");
                break;
            case 4:
                break;
        }
    } while (choice != 4);
    
    freeMovies(movies);
    return EXIT_SUCCESS;
}
