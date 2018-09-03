//
// Created by akursat on 01.09.2018.
//

#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include "ksnVector.h"

#ifndef TEKDERS_COMMON_H
#define TEKDERS_COMMON_H

#define PORT     8081
#define MAXLINE  1024

#define NAMESIZE 256
#define ALIAS_SIZE 1024

#define MAX_THREAD 10

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct myService {
    int sequece;
    int port;
    char names[NAMESIZE];
} hostserv;

typedef struct {
    int client_id;
    int sequence_number;
    char protocol_name[NAMESIZE];
    char service_name[NAMESIZE];
} request_t;

typedef struct {
    request_t request;
    struct sockaddr_in cli_adr;
    size_t cli_adr_len;
} ext_request_t;

typedef struct {
    int client_id;
    int sequence_number;
    int port_number;
    char aliases[ALIAS_SIZE];
} response_t ;

void debug_request(request_t *request);
void debug_ext_request(ext_request_t *ext_request);
void debug_response(response_t *response);
void debug_servent(struct servent *serv);

bool isValidIpAddress(char *ipAddress);

char* req2str(request_t *request);
request_t *str2req(char* string);

char* res2str(response_t *response);
response_t *str2res(char* string);

#endif //TEKDERS_COMMON_H

