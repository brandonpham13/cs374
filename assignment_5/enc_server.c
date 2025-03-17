/* enc_server.c - Encryption Server */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define MAX_BUFFER 100000
#define MAX_CONNECTIONS 5

void encrypt_text(char *plaintext, char *key, char *ciphertext) {
    for (size_t i = 0; i < strlen(plaintext); i++) {
        int p_val = (plaintext[i] == ' ') ? 26 : plaintext[i] - 'A';
        int k_val = (key[i] == ' ') ? 26 : key[i] - 'A';
        int c_val = (p_val + k_val) % 27;
        ciphertext[i] = (c_val == 26) ? ' ' : (char)(c_val + 'A');
    }
    ciphertext[strlen(plaintext)] = '\0';
}

void handle_client(int client_socket) {
    char plaintext[MAX_BUFFER], key[MAX_BUFFER], ciphertext[MAX_BUFFER];
    recv(client_socket, plaintext, sizeof(plaintext), 0);
    recv(client_socket, key, sizeof(key), 0);
    encrypt_text(plaintext, key, ciphertext);
    send(client_socket, ciphertext, strlen(ciphertext), 0);
    close(client_socket);
    exit(0);
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
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, MAX_CONNECTIONS);
    
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_size);
        if (fork() == 0) { 
            close(server_socket);
            handle_client(client_socket);
        }
        close(client_socket);
    }
    close(server_socket);
    return 0;
}