#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX_BUFFER 100000

void error(const char *msg) {
    perror(msg);
    exit(1);
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
}

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber, char* hostname) {
    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address));
    
    // The address should be network capable
    address->sin_family = AF_INET;
    
    // Store the port number
    address->sin_port = htons(portNumber);
    
    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0],hostInfo->h_length);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s ciphertext_file key_file port\n", argv[0]);
        exit(1);
    }

    int socketFD, charsWritten, charsRead;
    struct sockaddr_in server_addr;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) error("ERROR opening socket");

    setupAddressStruct(&server_addr, atoi(argv[2]), "localhost");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error("ERROR connecting");

    char ciphertext[MAX_BUFFER], key[MAX_BUFFER], plaintext[MAX_BUFFER];
    memset(plaintext, '\0', MAX_BUFFER);
    memset(ciphertext, '\0', MAX_BUFFER);
    memset(key, '\0', MAX_BUFFER);

    // Read ciphertext and key from files
    read_file(argv[1], ciphertext);
    read_file(argv[2], key);

    send(client_socket, ciphertext, strlen(ciphertext), 0);
    send(client_socket, key, strlen(key), 0);

    recv(client_socket, plaintext, MAX_BUFFER, 0);
    printf("%s\n", plaintext);
    close(client_socket);
    return 0;
}