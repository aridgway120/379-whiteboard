#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define	 MY_PORT  1111

#define MORE_TEXT 1
#define DONE 0

#define ENTRY_SIZE 1024
#define BUFF_SIZE 1024

/* ---------------------------------------------------------------------
 This is a sample client program for the number server. The client and
 the server need not run on the same machine.
 --------------------------------------------------------------------- */

int main(int argc, char* argv[])
{
	int MY_PORT = atoi(argv[1]);
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

	while (1) {
		
		fprintf (stderr, "Cmd::");
		fgets(in_buff, BUFF_SIZE, stdin);
		for (int i=0; i<BUFF_SIZE; i++) {
			if (in_buff[i] == 'z') {
				in_buff[i] = '\n';
			}
		}
		bytes_written = write(s, in_buff, strlen(in_buff));
		bzero(in_buff, BUFF_SIZE);

		bzero(s_buff, BUFF_SIZE);
		read(s, s_buff, BUFF_SIZE);
		fprintf(stderr, "Response from server:\"%s\"\n", s_buff);
	}

	close (s);

		//sleep (2);
	return 0;
}
