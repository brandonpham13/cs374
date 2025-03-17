/* enc_client.c - Encryption Client */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER 100000

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s plaintext key port\n", argv[0]);
        exit(1);
    }
    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[3]));
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    send(client_socket, argv[1], strlen(argv[1]), 0);
    send(client_socket, argv[2], strlen(argv[2]), 0);
    
    char ciphertext[MAX_BUFFER];
    recv(client_socket, ciphertext, sizeof(ciphertext), 0);
    printf("%s\n", ciphertext);
    close(client_socket);
    return 0;
}