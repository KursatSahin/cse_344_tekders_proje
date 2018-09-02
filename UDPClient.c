//
// Created by akursat on 31.08.2018.
//

#include "UDPClient.h"

// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Common.h"

// Driver code
int main(int argc, char *argv[]) {

    checkUsage(argc,argv);

    // Parse cmd arguments
    char hostname_of_server[256];
    int server_udp_port;
    int timeout;

    strcpy(hostname_of_server,argv[1]);
    server_udp_port = atoi(argv[2]);
    timeout = atoi(argv[3]);

    char promt_protocol[NAMESIZE];
    char promt_service_name[NAMESIZE];

    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t slen = sizeof(servaddr);

    // Promt host informations
    printf("\nEnter protocol : ");
    scanf("%s",promt_protocol);
    printf("\nEnter service name : ");
    scanf("%s",promt_service_name);

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

    //socklen_t len;
    ssize_t recv_len;
    ssize_t send_len;

    // Create request and response object
    request_t request = {80,"http","udp"};
    response_t response;
    // End of create request and response object

    // Display request
    debug_request(&request);

//    send_len = sendto(sockfd, (const char *) hello, strlen(hello),
//                      0, (const struct sockaddr *) &servaddr,
//                      slen);

    send_len = sendto(sockfd, (request_t*)&request, sizeof(request_t),
                      0, (const struct sockaddr *) &servaddr,
                      slen);

    if (send_len == -1) {
        perror("sendto()");
    }

    printf("Request sent.\n");

    recv_len = recvfrom(sockfd, &response, sizeof(response_t),
                        0, (struct sockaddr *) &servaddr,
                        &slen);
    if (recv_len == -1) {
        perror("recvfrom()");
        return (-1);
    }

    response.port_number = htons(response.port_number);

    debug_response(&response);

//    buffer[recv_len] = '\0';
//    printf("Server says : %s\n", buffer);

    close(sockfd);
    return 0;
}

void checkUsage(int argc, char *argv[]){
    if (argc != 4){
        fprintf(stderr, "usage: %s ip_adress port_num timeout(in seconds)\n or \nusage: %s localhost port_num timeout(in seconds)\n", argv[0], argv[0]);
        //exit(EXIT_FAILURE);
    } else {
        if (strcmp(argv[1],"localhost") || isValidIpAddress(argv[1])){
            fprintf(stderr, "Invalid ip adress\n");
            //exit(EXIT_FAILURE);
        }

        if (atoi(argv[2]) < 1024 && atoi(argv[2]) > 49151){
            fprintf(stderr, "Invalid port number\n");
            //exit(EXIT_FAILURE);
        }

        if (atoi(argv[3]) == 0){
            fprintf(stderr, "Invalid timeout\n");
            //exit(EXIT_FAILURE);
        }
    }
}