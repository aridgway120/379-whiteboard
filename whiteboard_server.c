#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARG_NO 4

#define NEW 1
#define LOAD 2

#define ENTRY_SIZE 512

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
        int MAX_ENTRIES = atoi(argv[3]);

        char** whiteboard = malloc(MAX_ENTRIES * sizeof(char*));
        for (int i=0; i < MAX_ENTRIES; i++) {
            whiteboard[i] = malloc(ENTRY_SIZE);
            strcpy(whiteboard[i], "!12e0\n\n");
        }
    }
    else {
        int a;
    }

    char MAX_ENTRIES_STR[] = "20";
    int MAX_ENTRIES = atoi(MAX_ENTRIES_STR);
    int n_entries = 0;
    char* CONNECTION_MSG = malloc(33 + (int)log10(MAX_ENTRIES));
    strcpy(CONNECTION_MSG, "CMPUT379 Whiteboard Server v0\n");
    strcat(CONNECTION_MSG, MAX_ENTRIES_STR);
    strcat(CONNECTION_MSG, "\n");

    // Startup procedures
    int sock, snew, bind_result, clientlength;

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

    listen(sock, 20);

    // Ongoing processes
    while(1) {
        clientlength = sizeof(client);
        snew = accept(sock, (struct sockaddr*)&client, (socklen_t*)&clientlength);
        if (snew < 0) {
            perror("Server: Accept failed.");
            exit(1);
        }
        int bytes_written = write(snew, CONNECTION_MSG, strlen(CONNECTION_MSG));
        close(snew);
        
    }
}