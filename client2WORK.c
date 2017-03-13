#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define	 MY_PORT  1111

#define MORE_TEXT 1
#define DONE 0


#define BUFF_SIZE 1050


/* ---------------------------------------------------------------------
 This is a sample client program for the number server. The client and
 the server need not run on the same machine.
 --------------------------------------------------------------------- */

int main()
{
	int	s, number;
	int bytes_read, bytes_written;

	char s_buff[BUFF_SIZE];
	bzero(s_buff, BUFF_SIZE);
	char in_buff[BUFF_SIZE];
	bzero(in_buff, BUFF_SIZE);

	struct	sockaddr_in	server;

	struct	hostent		*host;

	host = gethostbyname ("127.0.0.1");

	if (host == NULL) {
		perror ("Client: cannot get host description");
		exit (1);
	}


	s = socket (AF_INET, SOCK_STREAM, 0);

	if (s < 0) {
		perror ("Client: cannot open socket");
		exit (1);
	}

	bzero (&server, sizeof (server));
	bcopy (host->h_addr, & (server.sin_addr), host->h_length);
	server.sin_family = host->h_addrtype;
	server.sin_port = htons (MY_PORT);

	if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
		perror ("Client: cannot connect to server");
		exit (1);
	}

	bytes_read = read (s, s_buff, BUFF_SIZE-1);
	s_buff[strlen(s_buff)-1] = ' ';

	fprintf (stderr, "Welcome to %sentry whiteboard.\n\n", s_buff);

	char message[999];
	char buffer[1];
	char entry[99];
	char to_send[BUFF_SIZE];
	bzero(to_send, BUFF_SIZE);
	char response[BUFF_SIZE];
	bzero(response, BUFF_SIZE);
	char inbuff[200];
	char substring_to_find[10];
	char substring_to_replace[10];
	
	char Q_or_U_or_C;
	char C_or_P;
	char mode;
	char token;
	char newline = '\n';
	
	int rc;
	int rc2;
	int len;
	
	long long num_entry;
	
	
	strcpy(substring_to_find, "\\n");
	strcpy(substring_to_replace, "\n");


	while (1) {
		//fprintf (stderr, "Cmd::");
		//fgets(in_buff, BUFF_SIZE, stdin);
		//for (int i=0; i<BUFF_SIZE; i++) {
		//	if (in_buff[i] == 'z') {
		//		in_buff[i] = '\n';
		//	}
		//}
		//bytes_written = write(s, to_send, strlen(to_send)-1);
		//bzero(to_send, BUFF_SIZE);

		//bzero(s_buff, BUFF_SIZE);
		//read(s, s_buff, BUFF_SIZE);
		//fprintf(stderr, "Response from server:\"%s\"\n", s_buff);
		
		printf("\nWould you like to query, update or clean an entry or exit? (q/u/c/e): ");
		fgets(inbuff, 200, stdin);
		
		rc = sscanf(inbuff, "%c", &Q_or_U_or_C);
		
		if (rc != 1 || (strcmp(&Q_or_U_or_C, "e") != 0 && strcmp(&Q_or_U_or_C, "E") && strcmp(&Q_or_U_or_C, "q") != 0 && strcmp(&Q_or_U_or_C, "Q") != 0 && strcmp(&Q_or_U_or_C, "u") != 0 && strcmp(&Q_or_U_or_C, "U") != 0 && strcmp(&Q_or_U_or_C, "c") != 0  && strcmp(&Q_or_U_or_C, "C") != 0)) { //if it didn't get any input
			fprintf(stderr, "Incorrect entry for querying, updating or clearing an entry or exiting.\n");
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}

			printf("\nRestarting...\n");
			continue;
		}
		
		printf("Received %c.\n", Q_or_U_or_C);
		
		
		
		
		
		
		if (strcmp(&Q_or_U_or_C, "e") == 0 || strcmp(&Q_or_U_or_C, "E") == 0) {
		//EXITING
			printf("Exiting...\n");
			break;
		}
		
		
		
		
		if (strcmp(&Q_or_U_or_C, "c") == 0 || strcmp(&Q_or_U_or_C, "C") == 0) {
		//CLEANING
			token = '@';
			CLEANING_FLAG = 1;
			printf("Which entry would you like to update? (1 to %d): ", num_from_server);
			fgets(inbuff, 200, stdin);
			rc = sscanf(inbuff, "%s", entry);
			
			if (strcmp(entry, "\n") != 0) { //if the entry is empty, string_is_number has nothing to work with
			//string is not empty
				rc2 = string_is_number(entry);
			
				if (rc != 1 || rc2 == 0) {
					fprintf(stderr, "Failed to get correct entry\n\n");
					printf("\nrestarting\n");
					
					if (feof(stdin)) { //if there's still elements in stdin that need to be read
						while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
					}
					
					continue;
				}
				else {printf("string is a number\n");}
			}
			else if (strcmp(entry, "\n") == 0) {
			//string is emtpy
				if (rc != 1 ) {
					fprintf(stderr, "Failed to get correct entry\n\n");
					printf("\nrestarting\n");
					
					if (feof(stdin)) { //if there's still elements in stdin that need to be read
						while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
					}
					
					continue;
				}
			}
				
			
			printf("received %s\n", entry);
			
			num_entry = atoi(entry);
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}
			//@12p0\n\n
			sprintf(to_send, "%c%lld%c%d%c%c", token, num_entry, 'p', 0, newline, newline);
			printf("SENDING %s", to_send);
			bytes_written = write(s, to_send, strlen(to_send)-1);
			bzero(to_send, BUFF_SIZE);
			read(s, response, BUFF_SIZE);
			fprintf(stderr, "Response from server:\"%s\"\n", s_buff);
		}
			
			
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		if (strcmp(&Q_or_U_or_C, "q") == 0 || strcmp(&Q_or_U_or_C, "Q") == 0) {
			//QUERYING
			token = '?';
			
			QUERY_FLAG = 1;  //these are being used to logically separate how the responses are parsed in the final step, in the real
			//program these are unneeded as the final functions would just be part of these first update or query functions, they'd block until they receive the
			//server response
			
			
			//would be great to replace this with fgets, because a sequence of events that can break all these scanf statements is if they hit enter, then type something
			printf("Which entry would you like to query? (1 to %d): ", num_from_server);
			fgets(inbuff, 200, stdin);
			rc = sscanf(inbuff, "%s", entry);
			
			if (strcmp(entry, "\n") != 0) { //if the entry is empty, string_is_number has nothing to work with
			//string is not empty
				rc2 = string_is_number(entry);
			
				if (rc != 1 || rc2 == 0) {
					fprintf(stderr, "Failed to get correct entry\n\n");
					printf("\nrestarting\n");
					
					if (feof(stdin)) { //if there's still elements in stdin that need to be read
						while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
					}
					
					continue;
				}
				else {printf("string is a number\n");}
			}
			else if (strcmp(entry, "\n") == 0) {
			//string is emtpy
				if (rc != 1 ) {
					fprintf(stderr, "Failed to get correct entry\n\n");
					printf("\nrestarting\n");
					
					if (feof(stdin)) { //if there's still elements in stdin that need to be read
						while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
					}
					
					continue;
				}
			}
				
			
			printf("received %s\n", entry);
			
			num_entry = atoi(entry);
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}
			
			//construct the response
			sprintf(to_send, "%c%lld%c", token, num_entry, newline);
			printf("SENDING %s", to_send);
			bytes_written = write(s, to_send, strlen(to_send)-1);
			bzero(to_send, BUFF_SIZE);
			read(s, response, BUFF_SIZE);
			fprintf(stderr, "Response from server:\"%s\"\n", s_buff);
		}
		
		
		
		
		
		
		
		
		
		
		else if (strcmp(&Q_or_U_or_C,"u") == 0 || strcmp(&Q_or_U_or_C, "U") == 0) {
			//UPDATING
			UPDATE_FLAG = 1;  //these are being used to logically separate how the responses are parsed in the final step, in the real
			//program these are unneeded as the final functions would just be part of these first update or query functions, they'd block until they receive the
			//server response
			token = '@';
			
			printf("Which entry would you like to update? (1 to %d): ", num_from_server);
			fgets(inbuff, 200, stdin);
			rc = sscanf(inbuff, "%s", entry);
			
			if (strcmp(entry, "\n") != 0) { //if the entry is empty, string_is_number has nothing to work with
			//string is not empty
				rc2 = string_is_number(entry);
			
				if (rc != 1 || rc2 == 0) {
					fprintf(stderr, "Failed to get correct entry\n\n");
					printf("\nrestarting\n");
					
					if (feof(stdin)) { //if there's still elements in stdin that need to be read
						while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
					}
					
					continue;
				}
				else {printf("string is a number\n");}
			}
			else if (strcmp(entry, "\n") == 0) {
			//string is emtpy
				if (rc != 1 ) {
					fprintf(stderr, "Failed to get correct entry\n\n");
					printf("\nrestarting\n");
					
					if (feof(stdin)) { //if there's still elements in stdin that need to be read
						while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
					}
					
					continue;
				}
			}
				
			
			printf("received %s\n", entry);
			
			num_entry = atoi(entry);
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}
			
			printf("What would you like your message to be? \nYou may enter \\n for a newline\n(will accept up to 999 characters the rest are truncated): ");
			fgets(message, 999, stdin); 
			
			
			if (rc != 1) {
				fprintf(stderr, "Failed to receive message\n\n");
				printf("\nrestarting\n");
				if (feof(stdin)) { //if there's still elements in stdin that need to be read
					while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
				}
				continue;
			}

			strreplace(message, substring_to_find, substring_to_replace);
			printf("message received: %s", message);	
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}	
		
			printf("Would you like to encrypt that message? (y/n): ");
			//when implemented exactly as above with the Q_or_U_or_C, it doesn't ever succeed
			rc = scanf("%s", &C_or_P);
			getchar();
			if (rc != 1  || (strcmp(&C_or_P, "y") != 0 && strcmp(&C_or_P, "n") != 0 && strcmp(&C_or_P, "Y") != 0 && strcmp(&C_or_P, "N") != 0 )) {
				fprintf(stderr, "Failed to get correct encryption answer\n\n");
				printf("\nrestarting\n");
				continue;
			}

			printf("received %c\n", C_or_P);
			
			//setting mode by default to p, if they answered yes, then it will be changed to encryption and the message will be encrypted
			mode = 'p';
			if (strcmp(&C_or_P, "y") == 0 || strcmp(&C_or_P, "Y") == 0) {
				mode = 'c';
				//message = encrypt(message);
			}
			
			printf("set mode to %c\n", mode);
			
			len = strlen(message) - 1;
			
			
			sprintf(to_send, "%c%lld%c%d%c%s%c", token, num_entry, mode, len, newline, message, newline);
			printf("SENDING %s", to_send);
			bytes_written = write(s, to_send, strlen(to_send)-1);
			bzero(to_send, BUFF_SIZE);
			read(s, response, BUFF_SIZE);
			fprintf(stderr, "Response from server:\"%s\"\n", s_buff);
			
		}
		
		
		
		
		
		
		
		
		
		
	}

	close (s);
	return 0;
}
