/* keygen.c - Generates a random key */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHARSET "ABCDEFGHIJKLMNOPQRSTUVWXYZ "

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s keylength\n", argv[0]);
        exit(1);
    }
    
    int key_length = atoi(argv[1]);
    if (key_length < 1) {
        fprintf(stderr, "Key length must be greater than 0.\n");
        exit(1);
    }
    
    srand(time(NULL));
    for (int i = 0; i < key_length; i++) {
        putchar(CHARSET[rand() % 27]);
    }
    putchar('\n');
    return 0;
}