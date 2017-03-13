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

#define	 MY_PORT  1277



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
		
		printf("Received %c.\n", Q_or_U_or_C);
		
		
		
		
		
		
		if (Q_or_U_or_C == 'e' || Q_or_U_or_C == 'E') {
		//EXITING
			printf("Exiting...\n");
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
			//bytes_written = write(s, to_send, BUFF_SIZE);
			bzero(to_send, BUFF_SIZE);
			read(s, response, BUFF_SIZE);
			fprintf(stderr, "Response from server:\"%s\"\n", response);
			
			
			
			
			
			
			//COULD RECEIVE ONE OF TWO THINGS:
			//!47e14\nNo such entry!\n 
			//which indicates we queried outside of the range 
			//!12e0\n\n
			//which indicates that the update was successful
			//it's safe to ignore the last message, or if you want, understand that if the length is 0 then it was a success, 
			//and respond to the user telling them that it was successful
			
			sscanf(response, "%c%lld%c%d%c%c", &token, &num_entry, &mode, &len, &newline, &start_of_response_message);
			printf("Client understands:\n");
			printf("Entry number: %lld\n", num_entry);
			printf("Mode: %c\n", mode);
			printf("Message length: %d\n", len);
			printf("first character of message: %c\n", start_of_response_message); //if it's '\n' then we have gotten a success, otherwise there was an error and we must terminate
			
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
			
			
			
			
			

			fprintf(stderr, "Response from server:\"%s\"\n", response);
			
			
			
			
			

            
            
            //to rule out other parts of the code affecting these variables
            char retoken;
            long long renum_entry;
            char remode;
            int relen;
            char in_here[1024];
            
            
			
			if (response[0] == '!') {
				//catch ![num_entry][mode][length]\n but for some reason, doesn't catch length properly, even though I do this for literally every other one
				sscanf(response, "%c%lld%c%d%c", &retoken, &renum_entry, &remode, &relen, &newline);
				int enSize = (renum_entry<=0) ? 1 : ((int) log10(renum_entry) + 1);
            	// missing entry length error
            	if (len == -1) {
                	fprintf(stderr, "Fatal Error! Entry length missing!\n");
                	break;
            	}
            	int elSize = (relen==0) ? 1 : ((int) log10(relen) + 1);
            
            	int entry_start = 1+enSize+1+elSize+1; // index at which the entry text starts
            	
            	
				if (relen == 0) {
				//we got a blank message as the return
				//!12p0\n\n
					printf("Client understands:\n");
					printf("Entry number: %lld\n", renum_entry);
					printf("Mode: %c\n", remode);
					printf("Message length: %d\n", relen); //In all the other sscanf calls, it has no problem extracting the correct length, but for some reason this one extracts much more than a number
					printf("This entry is blank!\n");
				}
				else {
					//we got a correct entry or we got an error
					//!12p30\nthisisaresponsetodemothelength\n or
					//!12e14\nNo such entry!\n
					strncat(in_here, response+entry_start, relen); // add entry text //I KNOW I should be using strncpy, but when I do, it throws an abort trap: 6, 
					printf("Client understands:\n");               // I'm guessing because of relen being enormous
					printf("Entry number: %lld\n", renum_entry);
					printf("Mode: %c\n", remode);
					printf("Message length: %d\n", relen);
					printf("Message: %s\n", in_here); 
			
					printf("Print to the user: %s\n", in_here);
					if (remode == 'c') {
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
			fprintf(stderr, "Response from server:\"%s\"\n", response);
			
			//COULD RECEIVE ONE OF TWO THINGS:
			//!47e14\nNo such entry!\n 
			//which indicates we queried outside of the range 
			//!12e0\n\n
			//which indicates that the update was successful
			//it's safe to ignore the last message, or if you want, understand that if the length is 0 then it was a success, 
			//and respond to the user telling them that it was successful
			
			sscanf(response, "%c%lld%c%d%c%c", &token, &num_entry, &mode, &len, &newline, &start_of_response_message);
			printf("Client understands:\n");
			printf("Entry number: %lld\n", num_entry);
			printf("Mode: %c\n", mode);
			printf("Message length: %d\n", len);
			printf("first character of message: %c\n", start_of_response_message); //if it's '\n' then we have gotten a success, otherwise there was an error and we must terminate
			
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
	fprintf(stderr, "Exiting...\n");
	close (s);
	return 0;
}
