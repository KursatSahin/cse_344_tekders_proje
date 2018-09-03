//
// Created by akursat on 31.08.2018.
//


// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <errno.h>

#include "Common.h"
#include "UDPServer.h"

// GLOBAL VARS
int sockfd;
vector thread_vector;

// Driver code
int main() {
    struct sockaddr_in servaddr, cliaddr;
    socklen_t slen = sizeof(cliaddr);

    int status;

    vector_init(&thread_vector);

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
    request_t request = {0};
    ext_request_t ext_request = {0};

    // Signal handler
    signal(SIGINT, INThandler);

    // TODO : while loop
    while (1) {

        printf("\n\nWaiting for data...\n\n");

        recv_len = recvfrom(sockfd, &request, sizeof(request_t), 0, (struct sockaddr *) &cliaddr, &slen);
        if (recv_len == -1) {
            perror("recvfrom");
            return -1;
        }

        // Display request
        debug_request(&request);

        // Create extended request from request
        ext_request.request = request;
        ext_request.cli_adr = cliaddr;
        ext_request.cli_adr_len = slen;

        // Display extended request
        debug_ext_request(&ext_request);

        pthread_t *thread = calloc(1, sizeof(pthread_t));
        if (thread == NULL) {
            handle_error("thread calloc failed");
        }

        status = pthread_create(thread, NULL, thread_func, &ext_request);
        if (status != 0) {
            handle_error_en(status, "pthread_create");
        }
        vector_add(&thread_vector, thread);
        sleep(6);
    }

    return 0;
}

void INThandler(int sig) {
    signal(sig, SIG_IGN);
    fprintf(stderr,"Server terminated..\n");
    close(sockfd);
    for (int i = 0; i < vector_get_size(&thread_vector); ++i) {
        pthread_t *currentThread = vector_get(&thread_vector, i);
        int status = pthread_cancel(*currentThread);
        if (status != 0) {
            printf("can't cancel a thread");
        }
    }
    vector_free(&thread_vector);
    exit(0);
}

void *thread_func(void *arg) {
    ext_request_t *ext_request = (ext_request_t *) arg;
    ssize_t send_len;
    response_t response = {0};
// Create servent variable and check service availability with getservbyname()
    struct servent *serv;
    serv = getservbyname("http", "tcp");

    if (serv == NULL) {
        printf("Service not found\n");
    }

    // Display servent
    // debug_servent(serv);

    // Create response via servent variable
    response.sequence_number = ext_request->request.sequence_number;
    response.port_number = serv->s_port;

    int index = 0;
    while(serv->s_aliases[index] != NULL){
        strcat(response.aliases, serv->s_aliases[index]);
        ++index;
    }

    // snprintf(response.aliases, ALIAS_SIZE, "xxx, yyy, zzz");

    // Display response
    debug_response(&response);

    send_len = sendto(sockfd, &response, sizeof(response_t), 0, (const struct sockaddr *) &ext_request->cli_adr,
                      ext_request->cli_adr_len);
    //send_len = sendto(sockfd, (const char *) hello, strlen(hello), 0, (const struct sockaddr *) &cliaddr, slen);
    if (send_len == -1) {
        handle_error_en(send_len, "sendto()");
    }

    pthread_t self = pthread_self();
    for (int i = 0; i < vector_get_size(&thread_vector); ++i) {
        pthread_t *current = vector_get(&thread_vector, i);
        if (self == *current) {
            vector_remove(&thread_vector, i);
        }
    }

    return NULL;
}