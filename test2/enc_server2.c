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

void receive_full_message(int socket, char *buffer, int expected_size) {
    int total_received = 0;
    int bytes_received;
    while (total_received < expected_size) {
        bytes_received = recv(socket, buffer + total_received, expected_size - total_received, 0);
        if (bytes_received < 0) error("ERROR reading from socket");
        if (bytes_received == 0) break;
        total_received += bytes_received;
    }
    buffer[expected_size] = '\0';
}

int main(int argc, char *argv[]) {
    int listenSocket, connectionSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t client_size = sizeof(clientAddress);
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        error("ERROR opening socket");
    }

    // Allow port reuse
    int optval = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    setupAddressStruct(&serverAddress, atoi(argv[1]));

    if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        error("ERROR on binding");

    listen(listenSocket, MAX_CONNECTIONS);

    while (1) {
        connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &client_size);
        if (connectionSocket < 0) {
            error("ERROR on accept");
        }

        char plaintext[MAX_BUFFER], key[MAX_BUFFER], ciphertext[MAX_BUFFER];
        memset(plaintext, '\0', MAX_BUFFER);
        memset(key, '\0', MAX_BUFFER);

        size_t total_rcv = 0, bytes_rcv;
        while (total_rcv < strlen(plaintext)) {
            bytes_rcv = recv(connectionSocket, plaintext + total_rcv, strlen(plaintext) - total_rcv, 0);
            if (bytes_rcv < 0) error("ERROR reading from socket");
            total_rcv += bytes_rcv;
        }
        
        total_rcv = 0;
        while (total_rcv < strlen(key)) {
            bytes_rcv = recv(connectionSocket, key + total_rcv, strlen(key) - total_rcv, 0);
            if (bytes_rcv < 0) error("ERROR reading from socket");
            total_rcv += bytes_rcv;
    }

        encrypt_text(plaintext, key, ciphertext);
        send(connectionSocket, ciphertext, strlen(ciphertext), 0);

        close(connectionSocket);
    }
    close(listenSocket);
    return 0;
}