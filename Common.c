//
// Created by akursat on 31.08.2018.
//

#include "ksnVector.h"

#define NAMESIZE 256

struct myService {
    int sequece;
    int port;
    char names[NAMESIZE];
} hostserv;

typedef struct {
    int sequence_number;
    char name[NAMESIZE];
    char service_name[NAMESIZE];
} request_t;

typedef struct {
    int sequenceNumber;
    int port_number;
    vector *aliases;
} response_t ;

