//
// Created by akursat on 31.08.2018.
//

#include "UDPServer.h"

// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Common.h"


// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from server\0";
    struct sockaddr_in servaddr, cliaddr;
    size_t slen = sizeof(cliaddr);

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, slen);

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *) &servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
/*
    int llen=20;
    char cliaddr[llen];

    inet_ntop(AF_INET, &(cliaddr.sin_addr), cliaddr, llen);
    printf("address:%s\n",cliaddr);
*/

    ssize_t recv_len;
    ssize_t send_len;

    recv_len = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *) &cliaddr, &slen);
    if (recv_len == -1) {
        perror("recvfrom");
        return -1;
    }
//    printf("sinaddr: %d", cliaddr.sin_addr.s_addr);

    buffer[recv_len] = '\0';
    printf("Client : %s\n", buffer);

    send_len = sendto(sockfd, (const char *) hello, strlen(hello), 0, (const struct sockaddr *) &cliaddr, slen);
    if (send_len == -1) {
        perror("sendto");
        return -1;
    }

    printf("Hello message sent.\n");

    struct servent *serv;

    serv = getservbyname("http", "tcp");

    if (serv == NULL) {
        printf("Service not found\n");
        return EXIT_FAILURE;
    } else {
        printf("Name: %-15s  Port: %5d    Protocol: %-6s\n,   Aliases: %-40s",
               serv->s_name, ntohs(serv->s_port), serv->s_proto, serv->s_aliases[1]);
    }

    return 0;
}