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
int sockfd_gui;

int thread_count=0;
pthread_mutex_t thread_count_lock;

// Driver code
int main() {

    printf("\nServer started..\n");

    int client_count = 0;
    ssize_t send_len;

    if(pthread_mutex_init(&thread_count_lock,NULL) != 0){
        fprintf(stderr,"mutex init failed\n");
        exit(0);
    }

    struct sockaddr_in servaddr, servaddr_gui, cliaddr;
    socklen_t slen = sizeof(cliaddr);
    socklen_t slen_gui = sizeof(servaddr_gui);

    int status;

    // Creating socket file descriptor for clients
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("server socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, slen);
    memset(&servaddr_gui, 0, slen_gui);

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
    // End of creating socket file descriptor for clients

    // Creating socket file descriptor for gui
    if ((sockfd_gui = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("gui socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr_gui, 0, sizeof(servaddr_gui));

    // Filling server information
    servaddr_gui.sin_family = AF_INET; // IPv4
    servaddr_gui.sin_addr.s_addr = INADDR_ANY;
    servaddr_gui.sin_port = htons(GUI_PORT);

//    // Bind the socket with the server address
//    if (bind(sockfd_gui, (const struct sockaddr *) &servaddr_gui,
//             sizeof(servaddr_gui)) < 0) {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
    // End of creating socket file descriptor for gui

    // Create and initialize request and ext_request
    ssize_t recv_len;
    request_t request = {0};
    ext_request_t ext_request = {0};

    // Signal handler
    signal(SIGINT, INThandler);
    signal(SIGTERM, INThandler);
    signal(SIGHUP, INThandler);

    // TODO : while loop
    while (1) {

        //printf("\nWaiting for data...\n");

        recv_len = recvfrom(sockfd, &request, sizeof(request_t), 0, (struct sockaddr *) &cliaddr, &slen);
        if (recv_len == -1) {
            perror("recvfrom");
            return -1;
        }

        // Display request
//        debug_request(&request);

        if (request.client_id == -1){ // If a new client

            ssize_t send_len;

            response_t response = {0};
            response.client_id = ++client_count;

            send_len = sendto(sockfd, &response, sizeof(response_t), 0, (const struct sockaddr *) &cliaddr, slen);
            if (send_len == -1) {
                handle_error_en(send_len, "sendto()");
            }
        } else { // If an old client
            // Send gui info
            char gui_msg[1024] = {0};
            sprintf(gui_msg,"%d,%d,%s,%s",request.client_id,request.sequence_number,request.protocol_name,request.service_name);
            send_len = sendto(sockfd_gui, &gui_msg, sizeof(gui_msg), 0, (const struct sockaddr *) &servaddr_gui, slen_gui);
            if (send_len == -1) {
                handle_error_en(send_len, "sendto()");
            }

            printf("\nLog message sent gui\n");
            // End of send gui info


            // Create extended request from request
            ext_request.request = request;
            ext_request.cli_adr = cliaddr;
            ext_request.cli_adr_len = slen;

            // Display extended request
//            debug_ext_request(&ext_request);
            printf("\nRequest recieved from Client : %d \n",ext_request.request.client_id);

            //sleep(1);

            pthread_mutex_lock(&thread_count_lock);
            if (thread_count < MAX_THREAD) {
                pthread_mutex_unlock(&thread_count_lock);
                pthread_t *thread = calloc(1, sizeof(pthread_t));
                if (thread == NULL) {
                    handle_error("thread calloc failed");
                }

                status = pthread_create(thread, NULL, thread_func, &ext_request);
                if (status != 0) {
                    handle_error_en(status, "pthread_create");
                }
                pthread_mutex_lock(&thread_count_lock);
                ++thread_count;
            } else {
                //sleep(10);
                fprintf(stderr,"\nThread limit reached! Cannot serve any more requests\n");
            }

            pthread_mutex_unlock(&thread_count_lock);
        }

    }

    return 0;
}

void INThandler(int sig) {

    switch (sig) {
        case SIGINT:
        case SIGHUP:
        case SIGTERM:
            signal(sig, SIG_IGN);
            fprintf(stderr,"\nServer terminated..\n");
            close(sockfd);
            pthread_mutex_destroy(&thread_count_lock);
            break;
        default:
            break;
    }

    exit(0);
}

void *thread_func(void *arg) {
    //sleep(10);
    ext_request_t *ext_request = (ext_request_t *) arg;
    ssize_t send_len;
    response_t response = {0};
// Create servent variable and check service availability with getservbyname()
    struct servent *serv;
    serv = getservbyname(ext_request->request.service_name, ext_request->request.protocol_name);

    if (serv == NULL) {
        printf("Service not found\n");
        response.sequence_number = ext_request->request.sequence_number;
        response.port_number = -1;
    } else {
        // Create response via servent variable
        response.sequence_number = ext_request->request.sequence_number;
        response.port_number = serv->s_port;

        int index = 0;
        while(serv->s_aliases[index] != NULL){
            strcat(response.aliases, serv->s_aliases[index]);
            ++index;
        }
    }

    // Display servent
    // debug_servent(serv);



    // Display response
//    debug_response(&response);
    printf("\nResponse sent to Client : %d \n",ext_request->request.client_id);

    send_len = sendto(sockfd, &response, sizeof(response_t), 0, (const struct sockaddr *) &ext_request->cli_adr,
                      ext_request->cli_adr_len);
    if (send_len == -1) {
        handle_error_en(send_len, "sendto()");
    }

    pthread_mutex_lock(&thread_count_lock);
    --thread_count;
    pthread_mutex_unlock(&thread_count_lock);


    pthread_exit(NULL);
}