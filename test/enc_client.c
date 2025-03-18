
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX_BUFFER 100000
#define VALID_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZ "

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Function to validate input characters
void validate_chars(const char *buffer, const char *filename) {
    for (size_t i = 0; i < strlen(buffer); i++) {
        if (strchr(VALID_CHARS, buffer[i]) == NULL) {
            fprintf(stderr, "ERROR: Invalid character in file %s\n", filename);
            exit(1);
        }
    }
}

// Function to read the contents of a file into a buffer
void read_file(const char *filename, char *buffer) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        error("ERROR opening file");
    }
    fgets(buffer, MAX_BUFFER, file);
    fclose(file);
    // Remove trailing newline if it exists
    buffer[strcspn(buffer, "\n")] = 0;
    validate_chars(buffer, filename);
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s plaintext_file key_file port\n", argv[0]);
        exit(1);
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) error("ERROR opening socket");

    struct sockaddr_in server_addr;
    struct hostent* server;
    server = gethostbyname("localhost");
    if (server == NULL) error("ERROR, no such host");

    memset((char*)&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[3]));
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error("ERROR connecting");

    // Validate that we are connected to enc_server
    char serverType[10] = {0};
    recv(client_socket, serverType, sizeof(serverType) - 1, 0);
    if (strcmp(serverType, "ENC") != 0) {
        fprintf(stderr, "ERROR: enc_client cannot connect to a non-enc_server\n");
        exit(2);
    }

    char plaintext[MAX_BUFFER], key[MAX_BUFFER];
    memset(plaintext, '\0', MAX_BUFFER);
    memset(key, '\0', MAX_BUFFER);

    // Read plaintext and key from files
    read_file(argv[1], plaintext);
    read_file(argv[2], key);

    // Validate key length
    if (strlen(key) < strlen(plaintext)) {
        fprintf(stderr, "ERROR: Key file is shorter than plaintext file\n");
        exit(1);
    }

    send(client_socket, plaintext, strlen(plaintext), 0);
    send(client_socket, key, strlen(key), 0);

    char ciphertext[MAX_BUFFER];
    memset(ciphertext, '\0', MAX_BUFFER);
    recv(client_socket, ciphertext, MAX_BUFFER, 0);
    printf("%s\n", ciphertext);
    close(client_socket);
    return 0;
}