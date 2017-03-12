#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//compiles with 
//gcc -o start start.c
//runs with
// ./start
//compiles with two warnings about my sprintf statements, these warnings do not affect the functioning of the program


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



int main() {
	int num_from_server = 99;
	int len_local_array = num_from_server;
	char local_array[len_local_array][999]; //len_local_array is the number of entries, each of them are 999 characters long
	char encryption_status[len_local_array];
	//initializing the array to some dummy values
		
	for (int i = 0; i < len_local_array; i++) {
		strcpy(local_array[i], "HELLO");
		encryption_status[i] = 'p';
	}
	
	int UPDATE_FLAG;
	int QUERY_FLAG;
	//start on client side
	char Q_or_U;
	char E_or_P;
	char message[999];
	char buffer[2];
	char mode;
	char newline = '\n';
	char entry[99];
	char token;
	char to_send[1050];
	char inbuff[200];
	
	
	int rc;
	int rc2;
	int len;
	long long num_entry; //long long because typing a number in like 1200 shouldn't cause it to fail
	
	printf("\n\nWE CAN ADD MORE ELEMENTS TO THIS UI LATER\n");
	while(1) {
		UPDATE_FLAG = 0;
		QUERY_FLAG = 0;
		
		printf("\nWould you like to query or update? (q/u): ");
		fgets(inbuff, 200, stdin);
		
		rc = sscanf(inbuff, "%c", &Q_or_U);
		
		if (rc != 1 || (strcmp(&Q_or_U, "q") != 0 && strcmp(&Q_or_U, "Q") != 0 && strcmp(&Q_or_U, "u") != 0 && strcmp(&Q_or_U, "U") != 0 )) { //if it didn't get any input
			fprintf(stderr, "Incorrect entry for querying or updating question.\n");
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}

			printf("\nRestarting...\n");
			continue;
		}
		
		printf("Received %c.\n", Q_or_U);
		
		
		
		
		
		
		if (strcmp(&Q_or_U, "q") == 0 || strcmp(&Q_or_U, "Q") == 0) {
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
		}
		
		
		
		
		
		
		
		
		
		
		else if (strcmp(&Q_or_U,"u") == 0 || strcmp(&Q_or_U, "U") == 0) {
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
			
			////////////////////////SHOULD MAKE IT SO THAT IF MESSAGE IS JUST A NEWLINE, IT BECOMES EMPTY
			printf("What would you like your message to be? (will accept up to 999 characters the rest are truncated): ");
			fgets(message, 999, stdin); 
			
			
			if (rc != 1) {
				fprintf(stderr, "Failed to receive message\n\n");
				printf("\nrestarting\n");
				if (feof(stdin)) { //if there's still elements in stdin that need to be read
					while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
				}
				continue;
			}
			
			printf("message received: %s", message);	
			
			if (feof(stdin)) { //if there's still elements in stdin that need to be read
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			}	
		
			printf("Would you like to encrypt that message? (y/n): ");
			//when implemented exactly as above with fgets for Q_or_U, it doesn't ever succeed
			rc = scanf("%c", &E_or_P);
			
			if (rc != 1  || (strcmp(&E_or_P, "y") != 0 && strcmp(&E_or_P, "n") != 0 && strcmp(&E_or_P, "Y") != 0 && strcmp(&E_or_P, "N") != 0 )) {
				fprintf(stderr, "Failed to get correct encryption answer\n\n");
				printf("\nrestarting\n");
				while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
				continue;
			}

			printf("received %c\n", E_or_P);
			
			while (fgets(buffer, sizeof buffer, stdin) != NULL && buffer[0] != '\n') {} //keeps reading until it gets a newline
			
			//setting mode by default to p, if they answered yes, then it will be changed to encryption and the message will be encrypted
			mode = 'p';
			if (strcmp(&E_or_P, "y") == 0 || strcmp(&E_or_P, "Y") == 0) {
				mode = 'e';
				//message = encrypt(message);
			}
			
			printf("set mode to %c\n", mode);
			
			len = strlen(message);
			
			////////////////////////SHOULD MAKE IT SO THAT IF MESSAGE IS JUST A NEWLINE, IT BECOMES EMPTY
			sprintf(to_send, "%c%lld%c%d%c%s%c", token, num_entry, mode, len, newline, message, newline);
			
			
		}
		
		printf("SENDING %s", to_send);
		printf("endofmessage");
		
	
	
	
	
	
		//server side now
		printf("\n\n\nSERVER SIDE\n\n\n");
	
	
	
	
	
		char received[1050];
		//ss variables are server_side variables, I just really didn't want to type out server_side each time
		char ssmessage[999];
		
		char sstoken;
		char ssnewline;
		char ssmode;
	
		long long ssentry;
		int sslen;
		
		//back variables are what are going to be sent back to the client
		char back_response[1050];
		char back_message[999];
		char back_token;
		char back_mode;
		long long back_entry;
		char back_newline = '\n';
		
		int back_message_len;
		
		
		
	
		strcpy(received, to_send);
		
	    
		printf("received %s\n", received);
	
		printf("Server understands: \n");
		if (received[0] == '?') {
			printf("Query Command:\n");
			rc = sscanf(received, "%c%lld%c", &sstoken, &ssentry, &ssnewline);
			printf("entry: %lld.\n", ssentry);
		
		
			ssentry--;
		
			if (ssentry < 0 || ssentry >= len_local_array) {
			//NOTE, THIS CAN'T JUST BE PRINTED, AS IT'LL PRINT ON THE SERVER SIDE, IT NEEDS TO BE SENT BACK AS AN ERROR TO THE CLIENT
				printf("Location requested is out of range for current instance of the data array\n"); //Don't actually print this when implemented, as it will print on server side
				//!47e14\nNo such entry!\n
				back_token = '!';
				back_entry = ssentry + 1;
				back_mode = 'e';
				strcpy(back_message, "No such entry!");
				back_message_len = strlen(back_message);
				
				sprintf(back_response, "%c%lld%c%d%c%s%c", back_token, back_entry, back_mode, back_message_len, back_newline, back_message, back_newline);
			}
			else {          //if (ssentry < len_local_array) {
			//!12p30\nthisisaresponsetodemothelength\n
				back_token = '!';
				back_entry = ssentry + 1;
				back_mode = encryption_status[ssentry];
				strcpy(back_message, local_array[ssentry]);
				back_message_len = strlen(back_message);
					
				
				sprintf(back_response, "%c%lld%c%d%c%s%c", back_token, back_entry, back_mode, back_message_len, back_newline, back_message, back_newline);
				
			}
		
			
		
		
		}
	
		if (received[0] == '@') {
			printf("Update Command:\n");
			//rc = sscanf(received, "%c%d%c%d%c%999c", &sstoken, &ssentry, &ssmode, &sslen, &ssnewline, &ssmessage);
			rc = sscanf(received, "%c%lld%c%d%c%999[^\n]%c", &sstoken, &ssentry, &ssmode, &sslen, &ssnewline, &ssmessage, &ssnewline);

			printf("entry: %lld.\n", ssentry);
			printf("mode: %c.\n", ssmode);
			printf("length of message: %d.\n", sslen);
			printf("message content: %s.\n", ssmessage);
		
		
			ssentry--;
			if (ssentry < 0 || ssentry >= len_local_array) {
				printf("Location requested is out of range for current instance of the data array\n"); //Don't actually print this when implemented, as it will print on server side
				//!47e14\nNo such entry!\n
				back_token = '!';
				back_entry = ssentry + 1;
				back_mode = 'e';
				strcpy(back_message, "No such entry!");
				back_message_len = strlen(back_message);
				
				sprintf(back_response, "%c%lld%c%d%c%s%c", back_token, back_entry, back_mode, back_message_len, back_newline, back_message, back_newline);
			}
			else {              //if (ssentry < len_local_array) {
				strcpy(local_array[ssentry], ssmessage);
				encryption_status[ssentry] = ssmode;
				printf("contents of array position %lld is now %s.\nIt's encryption status is %c.\n", ssentry, local_array[ssentry], encryption_status[ssentry]);
				
				back_token = '!';
				back_entry = ssentry + 1;
				back_mode = 'e';
				//"!12e0\n\n"
				
				sprintf(back_response, "%c%lld%c%d%c%c", back_token, back_entry, back_mode, 0, back_newline, back_newline);
			}
			
		}
		
		printf("sending response: %s", back_response);
		printf("endmessage");
		
		
		
		//client side again
		printf("\n\n\nCLIENT SIDE\n\n\n");
		
		
		
		char final_received[1050];
		char final_token;
		long long final_entry;
		char final_mode;
		int final_message_len;
		char final_newline;
		char final_message[999];
		
		char start_of_final_message; //used to determine if the update response is an error or a success
		
		strcpy(final_received, back_response);
		printf("Received %s", final_received);
		
		if (UPDATE_FLAG == 1) {
			//COULD RECEIVE ONE OF TWO THINGS:
			//!47e14\nNo such entry!\n 
			//which indicates we queried outside of the range 
			//"!12e0\n\n"
			//which indicates that the update was successful
			//it's safe to ignore the last message, or if you want, understand that if the length is 0 then it was a success, 
			//and respond to the user telling them that it was successful
			
			sscanf(back_response, "%c%lld%c%d%c%c", &final_token, &final_entry, &final_mode, &final_message_len, &final_newline, &start_of_final_message);
			printf("Client understands:\n");
			printf("Entry number: %lld\n", final_entry);
			printf("Mode: %c\n", final_mode);
			printf("Message length: %d\n", final_message_len);
			printf("first character of message: %c\n", start_of_final_message); //if this is 'N' then we have gotten an error, if it's '\n' then we have gotten a success
			
			if (start_of_final_message == '\n') {
				printf("Print to the user: Successfully updated entry\n");
			}
			else {
				printf("Print to the user: No such entry!\n");
			}
			
		}
		
		
		if (QUERY_FLAG == 1) {
			//Two possibilities, both share a format making this real easy
		
			//!12p30\nthisisaresponsetodemothelength\n
			sscanf(back_response, "%c%lld%c%d%c%999[^\n]%c", &final_token, &final_entry, &final_mode, &final_message_len, &final_newline, &final_message, &final_newline);
			printf("Client understands:\n");
			printf("Entry number: %lld\n", final_entry);
			printf("Mode: %c\n", final_mode);
			printf("Message length: %d\n", final_message_len);
			printf("Message: %s\n", final_message); 
			
			printf("Print to the user: %s\n", final_message);
			
		}
	
	} //end while statement from top of main
}//close main

/*
NOTE:

upon
*/
		