#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define	 MY_PORT  1285



#define MORE_TEXT 1
#define DONE 0


#define BUFF_SIZE 1050
//client can't detect when the server connection has terminated

/* ---------------------------------------------------------------------
 This is a sample client program for the number server. The client and
 the server need not run on the same machine.
 --------------------------------------------------------------------- */
//compile with gcc -o client client2WORK.c
//run with ./client
//runs on port 1112

void strreplace(char *src, char *str, char *rep)
//http://stackoverflow.com/questions/3659694/how-to-replace-substring-in-c
{
    char *p = strstr(src, str);
    do  
    {   
        if(p)
        {
            char buf[1024];
            memset(buf,'\0',strlen(buf));

            if(src == p)
            {
                strcpy(buf,rep);
                strcat(buf,p+strlen(str));  
            }
            else
            {
                strncpy(buf,src,strlen(src) - strlen(p));
                strcat(buf,rep);
                strcat(buf,p+strlen(str));
            }

            memset(src,'\0',strlen(src));
            strcpy(src,buf);
        }   

    }while(p && (p = strstr(src, str)));
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

int string_is_number(char input[]) {
//http://stackoverflow.com/questions/16644906/how-to-check-if-a-string-is-a-number
//modified to fit situation
	/* Strip trailing newline */
	size_t ln = strlen(input) - 1;
	if( input[ln] == '\n' ) input[ln] = '\0';

	/* Ensure that input is a number */
	for( int i = 0; i < ln; i++){
    	if( !isdigit(input[i]) ){
        	fprintf(stderr, "%c is not a number.\n", input[i]);
        	return 0;
    	}
	}
	return 1;
}


int main(int argc, char* argv[])
{
	
	if (argc >= 4) {
		printf("No keyfile support available, we didn't make it that far..\n");
		exit (1);
	}
	char hostname[99];
	int portnumber;
	if (argc == 3) {
		
		strcpy(hostname, argv[1]);
		portnumber = atoi(argv[2]);
	}
	else {
		printf("Wrong number of arguments\nCall with hostname portnumber\n");
		exit(1);
	}
		
		
		
		
	int	s, number;
	int bytes_read, bytes_written;

	char s_buff[BUFF_SIZE];
	bzero(s_buff, BUFF_SIZE);
	char in_buff[BUFF_SIZE];
	bzero(in_buff, BUFF_SIZE);

	struct	sockaddr_in	server;

	struct	hostent		*host;

	host = gethostbyname (hostname);

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
	server.sin_port = htons (portnumber);

	if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
		perror ("Client: cannot connect to server");
		exit (1);
	}

	bytes_read = read (s, s_buff, BUFF_SIZE-1);
	if (bytes_read == 0) {
		printf("Connection lost\n");
		exit(1);
	}
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
	char start_of_response_message;
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
		bzero(inbuff, sizeof inbuff);
		printf("\nWould you like to query, update or clean an entry or exit? (q/u/c/e): ");
		fgets(inbuff, 200, stdin);
		
		rc = sscanf(inbuff, "%c", &Q_or_U_or_C);

		if (rc != 1 || (Q_or_U_or_C != 'e' && Q_or_U_or_C != 'E' && Q_or_U_or_C != 'q' && Q_or_U_or_C != 'Q' && Q_or_U_or_C != 'u' && Q_or_U_or_C != 'U' && Q_or_U_or_C != 'c'  && Q_or_U_or_C != 'C' )) { //if it didn't get any input
			fprintf(stderr, "Incorrect entry for querying, updating or clearing an entry or exiting.\n");
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}

			printf("\nRestarting...\n");
			continue;
		}
		
		//printf("Received %c.\n", Q_or_U_or_C);
		
		
		
		
		
		
		if (Q_or_U_or_C == 'e' || Q_or_U_or_C == 'E') {
		//EXITING
			break;
		}
		
		
		
		
		if (Q_or_U_or_C == 'c' || Q_or_U_or_C == 'C') {
		//CLEANING
			token = '@';
			
			printf("Which entry would you like to update? (starts at 1): ");
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
				
			
			//printf("received %s\n", entry);
			
			num_entry = atoi(entry);
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}
			//@12p0\n\n
			sprintf(to_send, "%c%lld%c%d%c%c", token, num_entry, 'p', 0, newline, newline);
			//printf("SENDING %s", to_send);
			bytes_written = write(s, to_send, strlen(to_send)-1);
			//bytes_written = write(s, to_send, BUFF_SIZE);
			bzero(to_send, BUFF_SIZE);
			bytes_read = read(s, response, BUFF_SIZE);
			if (bytes_read == 0) {
				printf("Connection lost\n");
				break;
			}
			//fprintf(stderr, "Response from server:\"%s\"\n", response);
			
			
			
			
			
			
			//COULD RECEIVE ONE OF TWO THINGS:
			//!47e14\nNo such entry!\n 
			//which indicates we queried outside of the range 
			//!12e0\n\n
			//which indicates that the update was successful
			//it's safe to ignore the last message, or if you want, understand that if the length is 0 then it was a success, 
			//and respond to the user telling them that it was successful
			
			sscanf(response, "%c%lld%c%d%c%c", &token, &num_entry, &mode, &len, &newline, &start_of_response_message);
			//printf("Client understands:\n");
			//printf("Entry number: %lld\n", num_entry);
			//printf("Mode: %c\n", mode);
			//printf("Message length: %d\n", len);
			//printf("first character of message: %c\n", start_of_response_message); //if it's '\n' then we have gotten a success, otherwise there was an error and we must terminate
			
			if (start_of_response_message == '\n') {
				printf("\nSuccessfully updated entry\n");
			}
			else {
			//we've got an error
				sscanf(response, "%c%lld%c%d%c%999[^\n]%c", &token, &num_entry, &mode, &len, &newline, &message, &newline);
				fprintf(stderr, "Fatal Error! %s\n", message);
				break;
				
				
				
			}
		}
			
			
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		if (Q_or_U_or_C == 'q'  || Q_or_U_or_C == 'Q') {
			//QUERYING
			token = '?';
			
			
			
			
			//would be great to replace this with fgets, because a sequence of events that can break all these scanf statements is if they hit enter, then type something
			printf("Which entry would you like to query? (starts at 1): ");
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
				//else {printf("string is a number\n");}
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
				
			
			//printf("received %s\n", entry);
			
			num_entry = atoi(entry);
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}
			
			//construct the response
			sprintf(to_send, "%c%lld%c", token, num_entry, newline);
			//printf("SENDING %s", to_send);
			bytes_written = write(s, to_send, strlen(to_send)-1);
			bzero(to_send, BUFF_SIZE);
			bzero(response, BUFF_SIZE);
			bytes_read = read(s, response, BUFF_SIZE);
			if (bytes_read == 0) {
				printf("Connection lost\n");
				break;
			}
			
			

			//fprintf(stderr, "Response from server:\"%s\"\n", response);
			
			
            
            
			
			if (response[0] == '!') {
				//catch ![num_entry][mode][length]\n but for some reason, doesn't catch length properly, even though I do this for literally every other one
				sscanf(response, "%c%lld%c", &token, &num_entry, &mode);
				response[strlen(response)-1] = '\0';

				int string_start = 0;
				//we got a correct entry or we got an error
				//!12p30\nthisisaresponsetodemothelength\n or
				//!12e14\nNo such entry!\n
            	
				if (mode == 'e') {
					printf("ERROR:\n");
					string_start = 0;
            		for (int j = 0; j < strlen(response); j++) {
            			if (string_start == 1) {
            				fprintf(stderr, "%c", response[j]);
            			}
            			if (response[j] == '\n') {
            				string_start = 1;
            			}
            		}
            		
            		break;
				
				}
				else {

					
					printf("MESSAGE: ");
					int string_start = 0;
            		for (int j = 0; j < strlen(response); j++) {
            			if (string_start == 1) {
            				printf("%c", response[j]);
            			}
            			if (response[j] == '\n') {
            				string_start = 1;
            			}
            		}
			
					if (mode == 'c') {
						printf("Would need to decrypt this entry\n");
					}
				}
			}
			else {
				fprintf(stderr, "Fatal Error! Unrecognized prefix!\n");
				break;
			}
		}
		
		
		
		
		
		
		
		
		
		
		else if (Q_or_U_or_C == 'u' || Q_or_U_or_C == 'U') {
			//UPDATING
			
			token = '@';
			
			printf("Which entry would you like to update? (starts at 1): ");
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
				//else {printf("string is a number\n");}
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
				
			
			//printf("received %s\n", entry);
			
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

			//printf("received %c\n", C_or_P);
			
			//setting mode by default to p, if they answered yes, then it will be changed to encryption and the message will be encrypted
			mode = 'p';
			if (strcmp(&C_or_P, "y") == 0 || strcmp(&C_or_P, "Y") == 0) {
				mode = 'c';
				//message = encrypt(message);
			}
			
			//printf("set mode to %c\n", mode);
			
			len = strlen(message) - 1;
			
			
			sprintf(to_send, "%c%lld%c%d%c%s%c", token, num_entry, mode, len, newline, message, newline);
			//printf("SENDING %s", to_send);
			bytes_written = write(s, to_send, strlen(to_send)-1);
			bzero(to_send, BUFF_SIZE);
			bytes_read = read(s, response, BUFF_SIZE);
			if (bytes_read == 0) {
				printf("Connection lost\n");
				break;
			}
			//fprintf(stderr, "Response from server:\"%s\"\n", response);
			
			//COULD RECEIVE ONE OF TWO THINGS:
			//!47e14\nNo such entry!\n 
			//which indicates we queried outside of the range 
			//!12e0\n\n
			//which indicates that the update was successful
			//it's safe to ignore the last message, or if you want, understand that if the length is 0 then it was a success, 
			//and respond to the user telling them that it was successful
			
			sscanf(response, "%c%lld%c%d%c%c", &token, &num_entry, &mode, &len, &newline, &start_of_response_message);
			//printf("Client understands:\n");
			//printf("Entry number: %lld\n", num_entry);
			//printf("Mode: %c\n", mode);
			//printf("Message length: %d\n", len);
			//printf("first character of message: %c\n", start_of_response_message); //if it's '\n' then we have gotten a success, otherwise there was an error and we must terminate
			
			if (start_of_response_message == '\n') {
				printf("\nSuccessfully updated entry\n");
			}
			else {
			//we've got an error
				sscanf(response, "%c%lld%c%d%c%999[^\n]%c", &token, &num_entry, &mode, &len, &newline, &message, &newline);
				fprintf(stderr, "Fatal Error! %s\n", message);
				break;
				
				
				
			}
			
		}
		
		
		
		
		
		
		
		
		
		
	}
	fprintf(stderr, "\nExiting...\n");
	close (s);
	return 0;
}
