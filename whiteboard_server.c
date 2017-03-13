#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARG_NO 4

#define NEW 1
#define LOAD 2

#define ENTRY_SIZE 1024
#define BUFF_SIZE 1024

#define N_SOCKS 20

#define MORE_TEXT 1
#define DONE 0

// compile as: gcc whiteboard_server.c -o wbs379 -pthread
// call with: wbs379 portnumber {-f statefile | -n entries}
char* MAX_ENTRIES;
char** whiteboard;
pthread_mutex_t * mutex;
pthread_t * threads;

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

int getEntryNumber(char buffer[BUFF_SIZE], int ind)
{
    // extracts entry number from request
    // if there is no number at [1], returns -1.
    if (!isdigit(buffer[1])) {
        return -1;
    }

    int e_num = 0;
    while (1) {
        char c = buffer[ind];
        if (isdigit(c)) {
            e_num *= 10;
            e_num += atoi(&c);
        }
        else {
            break;
        }
        ind++;
    }
    return e_num;
}

int getEntryLen(char buffer[BUFF_SIZE], int entry_no)
{
    // extracts entry number from request
    int ind = 2;
    if (entry_no == 0) {
        ind += 1;
    }
    else { ind += (int) (log10(entry_no) + 1); }

    return getEntryNumber(buffer, ind);
}

void * clientHandler(void * arg)
{
    int snew = *((int *) arg);
    char* buffer = malloc(BUFF_SIZE);
    char* response = malloc(BUFF_SIZE);
    bzero(buffer, BUFF_SIZE);

    int bytes_written, bytes_read=1;

    char* CONNECTION_MSG = malloc(33 + (int)log10(atoi(MAX_ENTRIES)));
    strcpy(CONNECTION_MSG, "CMPUT379 Whiteboard Server v0\n");
    strcat(CONNECTION_MSG, MAX_ENTRIES);
    strcat(CONNECTION_MSG, "\n");

    bytes_written = write(snew, CONNECTION_MSG, strlen(CONNECTION_MSG));

    while (1) {
        bzero(buffer, BUFF_SIZE);
        bzero(response, BUFF_SIZE);
        bytes_read = read(snew, buffer, BUFF_SIZE-1);
        fprintf(stderr, "\n%d bytes read.\n", bytes_read);
        if (bytes_read <= 0) { break; }
        fprintf(stderr, "Got this:\n\"%s\"from client.\n", buffer);

        int entryNumber = getEntryNumber(buffer, 1);
        int entryIndex = entryNumber-1;
        // missing entry number error
        if (entryNumber == -1 && (buffer[0]=='?' || buffer[0]=='@') ) {
            bzero(response, BUFF_SIZE);
            sprintf(response, "!-e32\nEntry number invalid or missing.\n");
            write(snew, response, strlen(response));
            continue;
        }
        // entry number out of bounds error
        if (entryNumber > atoi(MAX_ENTRIES)) {
            //error handling here
            //eg: !47e14\nNo such entry!\n
            bzero(response, BUFF_SIZE);
            sprintf(response, "!%de14\nNo such entry.\n", entryNumber);
            write(snew, response, strlen(response));
            continue;
        }
        int enSize = (entryNumber<=0) ? 1 : ((int) log10(entryNumber) + 1);
        
        if (buffer[0] == '?') {
            // Query
            fprintf(stderr, "Query to entry %d\n", entryNumber);
            // if (entryNumber > atoi(MAX_ENTRIES)+1) {
            //     //error handling here
            //     //eg: !47e14\nNo such entry!\n
            //     bzero(response, BUFF_SIZE);
            //     sprintf(response, "!%de14\nNo such entry.\n", entryNumber);
            //     write(snew, response, strlen(response));
            //     continue;
            // }

            // clear response
            bzero(response, BUFF_SIZE);
            // get entry
            pthread_mutex_lock( &(mutex[entryIndex]) );
            strcpy(response, whiteboard[entryIndex]);
            pthread_mutex_unlock( &(mutex[entryIndex]) );

            write(snew, response, strlen(response));

        }
        else if (buffer[0] == '@') {
            // Update
            // format: @[entry]p[msglen]\nmessage\n
            int entryLen = getEntryLen(buffer, entryNumber);
            // missing entry length error
            if (entryLen == -1) {
                bzero(response, BUFF_SIZE);
                sprintf(response, "!%de28\nUpdate entry length missing.\n", entryNumber);
                write(snew, response, strlen(response));
                continue;
            }
            int elSize = (entryLen==0) ? 1 : ((int) log10(entryLen) + 1);
            char encryption_mode[1] = "p";////need to read this

            fprintf(stderr, "Update to entry %d; %d digits\nlen = %d; %d digits\n", entryNumber, enSize, entryLen, elSize);
            

            int buff_len = strlen(buffer);
            fprintf(stderr, "buffer len = %d\n", buff_len);

            int entry_start = 1+enSize+1+elSize+1; // index at which the entry text starts

            // Create new entry string
            char new_entry[ENTRY_SIZE];         // empty string
            bzero(new_entry, ENTRY_SIZE);

            strcpy(new_entry, "!");             // response flag

            char * temp = malloc(enSize);       // entry number
            sprintf(temp, "%d", entryNumber);
            strcat(new_entry, temp);
            free(temp);

            strcat(new_entry, encryption_mode); // encryption mode

            temp = malloc(elSize);              // entry length
            sprintf(temp, "%d", entryLen);
            strcat(new_entry, temp);
            free(temp);

            strcat(new_entry, "\n");            // add newline
            strncat(new_entry, buffer+entry_start, entryLen); // add entry text
            strcat(new_entry, "\n");            // add newline

            fprintf(stderr, "new entry: \"%s\"\n", new_entry);
            // update whiteboard
            pthread_mutex_lock( &(mutex[entryIndex]) );
            bzero(whiteboard[entryIndex], ENTRY_SIZE);
            strcpy(whiteboard[entryIndex], new_entry);
            pthread_mutex_unlock( &(mutex[entryIndex]) );

            // send empty error message
            bzero(response, BUFF_SIZE);
            sprintf(response, "!%de0\n\n", entryNumber);

            write(snew, response, strlen(response));
        }
        else {
            fprintf(stderr, "Unknown to entry %d\n", entryNumber);
            char c = buffer[0];
            bzero(response, BUFF_SIZE);
            sprintf(response, "!%de30\nInvalid command character \"%c\".\n", entryNumber, buffer[0]);
            write(snew, response, strlen(response));
            continue;
        }

    }

    close(snew);

    fprintf(stderr, "Socket closed.\n");

    return (void *) 0;
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
        mutex = malloc(atoi(MAX_ENTRIES) * sizeof(pthread_mutex_t));

        for (int i=0; i < atoi(MAX_ENTRIES); i++) {
            whiteboard[i] = malloc(ENTRY_SIZE);
            bzero(whiteboard[i], ENTRY_SIZE);
            strcpy(whiteboard[i], "!12p0\n\n");
            pthread_mutex_init(&(mutex[i]), NULL);
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

    threads = calloc(10, sizeof(pthread_t));

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
    int ind = 0, t_ind = 0;
    // Ongoing processes
    while(1) {
        clientlength = sizeof(client);
        
        snew[ind] = accept(sock, (struct sockaddr*)&client, (socklen_t*)&clientlength);
        if (snew[ind] < 0) {
            perror("Server: Accept failed.");
        }
        else {
            fprintf(stderr, "Client diverted to socket %d.\n", ind);
            pthread_create(&(threads[t_ind]), NULL, clientHandler, (void *) &(snew[ind]) );
            t_ind++;
            // if (t_ind >= sizeof(*threads)/sizeof(pthread_t)) {
            //     threads = realloc(threads, sizeof(*threads) + 100*sizeof(pthread_t));
            // }
            //clientHandler((void*) &(snew[ind]));
            ind++;
            if (ind >= N_SOCKS) { ind -= N_SOCKS; }
        }
        //
    }
}