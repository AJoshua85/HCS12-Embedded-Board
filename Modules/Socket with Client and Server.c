/*
 * server.c
 * Written by Peter Roeser, adapted by Michael Galle
 * This is a sample internet server application that will respond
 * to requests on port 5000
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 5550

int main (void)
{

	char buffer[BUFSIZ];	// Data buffer for communications
	int server_socket, client_socket;
	int client_len;
	struct sockaddr_in client_addr, server_addr;
	int len;

	/* obtain a socket for the server */

	if ((server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		printf ("grrr, can't get the server socket\n");
		return 1;
	}	/* endif */

	/*
	 * initialize our server address info for binding purposes
	 */

	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	server_addr.sin_port = htons (PORT);

	if (bind (server_socket, (struct sockaddr *)&server_addr, 
	sizeof (server_addr)) < 0) {
		printf ("grrr, can't bind server socket\n");
		close (server_socket);
		return 2;
	}	/* endif */

	/* start listening on the socket */
	if (listen (server_socket, 10) < 0) {
		printf ("grrr, can't listen on socket\n");
		close (server_socket);
		return 3;
	}	/* endif */

	/* accept a packet from the client */
	client_len = sizeof (client_addr);
	if ((client_socket = accept (server_socket, 
	(struct sockaddr *)&client_addr, &client_len)) < 0) {
	  printf ("grrr, can't accept a packet from client\n");
	  close (server_socket);
	return 4;
	}

        while(1) {
          memset(buffer, 0, sizeof(buffer));
	  read (client_socket, buffer, BUFSIZ);
	  printf("Message from client: %s\n", buffer);
	  if (strcmp(buffer, "quit") == 0) {
	    printf("Client has requested to quit, exiting application\n");
	    break;
	  }
	  memset(buffer, 0, sizeof(buffer));
	  printf ("Enter a message to send to the client (type 'quit' to exit): ");
	  fflush (stdout); 
	  fgets (buffer, sizeof (buffer), stdin); 
	  if (buffer[strlen (buffer) - 1] == '\n') {
	    buffer[strlen (buffer) - 1] = '\0'; 
          }
	  /* write data to client */
	  write (client_socket, buffer, strlen(buffer));
	  if (strcmp(buffer, "quit") == 0) {
	    printf("Exiting application\n");
	    break;
	  }  
	}
	close (client_socket); 
	return 0;
}



/*
 * client.c
 *
 * This is a sample internet client application that will talk
 * to the server s.c via port 5000
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 5550


int
main (int argc, char *argv[])
{
	char buffer[BUFSIZ];	// data buffer for communications
	int client_socket, len;
	int addr;
	struct sockaddr_in server_addr;
	struct hostent *host;

	if (argc != 2) {
		printf ("Please enter ./client and an IP address to connect to\n");
		return 1;
	}	

	/* determine host info for server name supplied */
	if ((host = gethostbyname (argv[1])) == NULL) {
		printf ("grrr, can't get host info!\n");
		return 2;
	}	/* endif */
	memcpy (&addr, host->h_addr, host->h_length);

	/* get a socket for communications */

	if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		printf ("grrr, can't get a client socket!\n");
		return 3;
	}	/* endif */

	/* initialize struct to get a socket to host */
	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = addr;
	server_addr.sin_port = htons (PORT);

	/* attempt a connection to server */
	if (connect (client_socket, (struct sockaddr *)&server_addr,
	sizeof (server_addr)) < 0) {
		printf ("grrr, can't connect to server!\n");
		close (client_socket);
		return 4;
	}	/* endif */

	
	/* Get message from client and send to server */
	while(1) {
	  memset(buffer, 0, sizeof(buffer));
	  printf ("Enter a message to send to the server (type 'quit' to exit): ");
	  fflush (stdout); 
	  fgets (buffer, sizeof (buffer), stdin); 
	  if (buffer[strlen (buffer) - 1] == '\n') {
	    buffer[strlen (buffer) - 1] = '\0'; 
          }
	  write (client_socket, buffer, strlen (buffer));
	  if (strcmp(buffer, "quit") == 0) {
	    printf("Exiting application\n");
	    break;
	  }
	  memset(buffer, 0, sizeof(buffer));
	  len = read (client_socket, buffer, sizeof (buffer));
	  printf ("Response from server: %s\n\n", buffer);
	  if (strcmp(buffer, "quit") == 0) {
	    printf("Server has decided to quit this connection, exiting application\n");
	    break;
	  }
	}
	close (client_socket);
	return 0;
}	/* end main */



