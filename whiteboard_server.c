#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define	PORT_NO	2222

int main()
{
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