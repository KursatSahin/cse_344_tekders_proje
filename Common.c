//
// Created by akursat on 31.08.2018.
//

#include "Common.h"


void debug_request(request_t *request) {
    fprintf(stderr, "=== Request Object Debug Information ===\n");
    fprintf(stderr, "client id: %d\n", request->client_id);
    fprintf(stderr, "sequence number: %d\n", request->sequence_number);
    fprintf(stderr, "protocol name: %s\n", request->protocol_name);
    fprintf(stderr, "service name: %s\n", request->service_name);
}

void debug_ext_request(ext_request_t *ext_request) {
    fprintf(stderr, "=== Ext Request Object Debug Information ===\n");
    fprintf(stderr, "client id: %d\n", ext_request->request.client_id);
    fprintf(stderr, "sequence number: %d\n", ext_request->request.sequence_number);
    fprintf(stderr, "protocol name: %s\n", ext_request->request.protocol_name);
    fprintf(stderr, "service name: %s\n", ext_request->request.service_name);
    fprintf(stderr, "client address: %s\n", inet_ntoa(ext_request->cli_adr.sin_addr));
    fprintf(stderr, "client address len: %zd\n", ext_request->cli_adr_len);
}

void debug_response(response_t *response) {
    fprintf(stderr, "=== Response Object Debug Information ===\n");
    fprintf(stderr, "sequence no: %d\n", response->sequence_number);
    fprintf(stderr, "port no: %d\n", response->port_number);
    fprintf(stderr, "aliases: %s\n", response->aliases);
}

void debug_servent(struct servent *serv) {
    fprintf(stderr, "=== Servent Object Debug Information ===\n");
    fprintf(stderr, "Name: %-15s  Port: %5d    Protocol: %-6s\n,   Aliases: %-40s\n", serv->s_name, ntohs(serv->s_port),
            serv->s_proto, serv->s_aliases[0]);

}

bool isValidIpAddress(char *ipAddress) {
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}


/*
char *req2str(request_t *request){
    return NULL;
}

request_t *str2req(char* string){
    return  NULL;
}

char *res2str(response_t *response){
    return  NULL;
}

response_t *str2res(char* string){
    return  NULL;
}
*/