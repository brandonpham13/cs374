#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFFER 100000
#define MAX_CONNECTIONS 5

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void setupAddressStruct(struct sockaddr_in* address, int portNumber) {
    memset((char*) address, '\0', sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons(portNumber);
    address->sin_addr.s_addr = INADDR_ANY;
}

void encrypt_text(char *plaintext, char *key, char *ciphertext) {
    for (size_t i = 0; i < strlen(plaintext); i++) {
        int p_val = (plaintext[i] == ' ') ? 26 : plaintext[i] - 'A';
        int k_val = (key[i] == ' ') ? 26 : key[i] - 'A';
        int c_val = (p_val + k_val) % 27;
        ciphertext[i] = (c_val == 26) ? ' ' : (char)(c_val + 'A');
    }
    ciphertext[strlen(plaintext)] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_size = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) error("ERROR opening socket");

    setupAddressStruct(&server_addr, atoi(argv[1]));

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        error("ERROR on binding");

    listen(server_socket, MAX_CONNECTIONS);

    // Allow port reuse
    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_size);
        if (client_socket < 0) error("ERROR on accept");

        // Send server type to client
        send(client_socket, "ENC", 4, 0);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_size);
        if (client_socket < 0) error("ERROR on accept");

        char plaintext[MAX_BUFFER], key[MAX_BUFFER], ciphertext[MAX_BUFFER];
        memset(plaintext, '\0', MAX_BUFFER);
        memset(key, '\0', MAX_BUFFER);

        recv(client_socket, plaintext, MAX_BUFFER, 0);
        recv(client_socket, key, MAX_BUFFER, 0);
        encrypt_text(plaintext, key, ciphertext);
        send(client_socket, ciphertext, strlen(ciphertext), 0);

        close(client_socket);
    }
    close(server_socket);
    return 0;
}