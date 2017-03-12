#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARG_NO 4

#define NEW 1
#define LOAD 2

#define ENTRY_SIZE 512

#define N_SOCKS 20
#define FREE -2

// compile as: gcc whiteboard_server.c -o wbs379 -pthread
// call with: wbs379 portnumber {-f statefile | -n entries}
char* MAX_ENTRIES;
char** whiteboard;

int isNumber(char arg[]) {
    for (int i=0; i<strlen(arg); i++) {
        if (isdigit(arg[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

int isPortNo(char arg[]) {
    if (strlen(arg) != 4) {
        return 0;
    }
    
    return isNumber(arg);
}

int getMode(char arg[]) {
    if (strlen(arg) != 2) { return -1; }

    if (strcmp(arg, "-f") == 0) {
        return LOAD;
    }
    if (strcmp(arg, "-n") == 0) {
        return NEW;
    }

    return -1;
}

int clientHandler(void * arg)
{
    int snew = *((int *) arg);

    char* CONNECTION_MSG = malloc(33 + (int)log10(atoi(MAX_ENTRIES)));
    strcpy(CONNECTION_MSG, "CMPUT379 Whiteboard Server v0\n");
    strcat(CONNECTION_MSG, MAX_ENTRIES);
    strcat(CONNECTION_MSG, "\n");

    int bytes_written = write(snew, CONNECTION_MSG, strlen(CONNECTION_MSG));

    
    close(snew);

    fprintf(stderr, "Socket closed.\n");

    return 0;
}

int main(int argc, char* argv[])
{
    int PORT_NO = 0000;

    if (argc > ARG_NO) {
        fprintf(stderr, "ERROR: Too many arguments entered; last %d will be ignored.\n", argc-ARG_NO);
    } else if (argc < ARG_NO) {
        fprintf(stderr, "FATAL ERROR: Arguments Missing.\n");
        return -1;
    }

    if (isPortNo(argv[1])) {
        PORT_NO = atoi(argv[1]);
    } else {
        fprintf(stderr, "FATAL ERROR: Invalid Port Number.\n");
        return -1;
    }

    int init_mode = getMode(argv[2]);
    if (init_mode == -1) {
        fprintf(stderr, "FATAL ERROR: Invalid Startup Mode \"%s\"\n", argv[2]);
        return -1;
    }

    if (init_mode == NEW) {
        if (isNumber(argv[3]) == 0) {
            fprintf(stderr, "FATAL ERROR: Invalid number of entries.\n");
            return -1;
        }
        MAX_ENTRIES = calloc(12, sizeof(char));
        strcpy(MAX_ENTRIES, argv[3]);

        whiteboard = malloc(atoi(MAX_ENTRIES) * sizeof(char*));
        for (int i=0; i < atoi(MAX_ENTRIES); i++) {
            whiteboard[i] = malloc(ENTRY_SIZE);
            strcpy(whiteboard[i], "!12e0\n\n");
        }
    }
    else {
        int a;
    }

    // Whiteboard format: !EFN\nMESSAGE\n
    // where E=entry no., N = entry len, MESSAGE = text entry,
    //  F = format encrypted (c) or plaintext (p) or error (e)
    // eg: !12p30\nthisisaresponsetodemothelength\n

    int n_entries = 0;

    // Startup procedures
    int sock, snew[N_SOCKS], bind_result, clientlength;

    for (int i=0; i < N_SOCKS; i++) { snew[i] = -1; }

    struct sockaddr_in master, client;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Server: Cannot open socket.");
        exit(1);
    }

    master.sin_family = AF_INET;
    master.sin_addr.s_addr = INADDR_ANY;
    master.sin_port = htons(PORT_NO);

    bind_result = bind(sock, (struct sockaddr*)&master, sizeof(master));
    if (bind_result) {
        perror("Server: Cannot bind socket.");
        exit(1);
    }

    listen(sock, N_SOCKS);
    int ind = 0;
    // Ongoing processes
    while(1) {
        clientlength = sizeof(client);
        int ind;
        
        snew[ind] = accept(sock, (struct sockaddr*)&client, (socklen_t*)&clientlength);
        if (snew[ind] < 0) {
            perror("Server: Accept failed.");
            //exit(1);
        }
        else {
            fprintf(stderr, "Client diverted to socket %d.\n", ind);
            clientHandler((void*) &(snew[ind]));
            ind++;
            if (ind >= 20) { ind -= 20; }
        }
        //
    }
}