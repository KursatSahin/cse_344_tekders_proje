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
#include <time.h>
#include <pthread.h>
#include "Common.h"

int sockfd;
time_t limit_time;
int timeout;

// Driver code
int main(int argc, char *argv[]) {
    pthread_t thread_id=0;

    // Signal handler
    signal(SIGINT, INThandler);

    checkUsage(argc,argv);

    srand(time(NULL));

    // Parse cmd arguments
    char hostname_of_server[256];
    int server_udp_port;

    strcpy(hostname_of_server,argv[1]);
    server_udp_port = atoi(argv[2]);
    timeout = atoi(argv[3]);

    while (1){
        if (thread_id == 0){
            int status = pthread_create(&thread_id,NULL,readServerThread,NULL);
            if (status != 0) {
                handle_error_en(status, "pthread_create");
            }
        }

        if (limit_time != 0 && difftime(time(NULL),limit_time) > 0 ){
            pthread_cancel(thread_id);
            thread_id = 0;
            limit_time = 0;
            printf("\ntimeout..\n\n");
        }
    }

    close(sockfd);
    return 0;
}

void INThandler(int sig) {
    signal(sig, SIG_IGN);
    fprintf(stderr,"Client terminated.\n");
    close(sockfd);
    exit(0);
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

void *readServerThread(void *arg){
    char promt_protocol[NAMESIZE];
    char promt_service_name[NAMESIZE];

    struct sockaddr_in servaddr;
    socklen_t slen = sizeof(servaddr);


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
    request_t request = {0};
    response_t response;

    while(1) {
        // Promt host informations
    //    printf("\nEnter protocol : ");
    //    scanf("%s",promt_protocol);
    //    printf("\nEnter service name : ");
    //    scanf("%s",promt_service_name);

        request.sequence_number = rand() % 1000;
        strcpy(request.protocol_name, "udp");
        strcpy(request.service_name, "http");

        // End of create request and response object

        // Display request
        debug_request(&request);

        send_len = sendto(sockfd, (request_t *) &request, sizeof(request_t),
                          0, (const struct sockaddr *) &servaddr,
                          slen);

        if (send_len == -1) {
            perror("sendto()");
        }

        printf("Request sent.\n");

        // Timeout init
        time_t current_time = time(NULL);
        limit_time = current_time + timeout;

        while(1) {
            recv_len = recvfrom(sockfd, &response, sizeof(response_t),
                                0, (struct sockaddr *) &servaddr,
                                &slen);
            if (recv_len == -1) {
                perror("recvfrom()");
                return NULL;
            }

            response.port_number = htons(response.port_number);

            // Sequence check
            if (response.sequence_number != request.sequence_number) {
                printf("Sequence number mismatch!!\n");
                debug_response(&response);
            } else {
                debug_response(&response);
                break;
            }

        }
        sleep(1);
    }
}