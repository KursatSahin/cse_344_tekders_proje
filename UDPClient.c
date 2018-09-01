//
// Created by akursat on 31.08.2018.
//

#include "UDPClient.h"

// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Common.h"

// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client\0";
    struct sockaddr_in servaddr;
    size_t slen = sizeof(servaddr);

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, slen);

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    socklen_t len;
    ssize_t recv_len;
    ssize_t send_len;

    send_len = sendto(sockfd, (const char *) hello, strlen(hello),
                      0, (const struct sockaddr *) &servaddr,
                      slen);
    if (send_len == -1) {
        perror("sendto");
    }
    printf("Hello message sent.\n");

    recv_len = recvfrom(sockfd, buffer, MAXLINE,
                        0, (struct sockaddr *) &servaddr,
                        &slen);
    if (recv_len == -1) {
        perror("recvfrom error");
        return (-1);
    }

    buffer[recv_len] = '\0';
    printf("Server says : %s\n", buffer);

    close(sockfd);
    return 0;
}
