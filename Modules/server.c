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
#include <termios.h>
#include <fcntl.h>
#include <errno.h>


#define BAUDRATE B9600
#define ERROR 	-1
#define DEVICE "/dev/ttyUSB0"
#define FAIL -1
#define MAX 32
#define PORT 5550

//Flags used to tell start and finish of msg
#define STARTFLAG "<" 
#define ENDFLAG  ">"


int openPort(void)
{
	int fd;
	fd = open(DEVICE,O_RDWR | O_NOCTTY | O_NDELAY);

	if(fd == ERROR)
	{
		printf("Cannot open port\n");
	}
	else
		printf("Port opened successfully\n");

	return(fd);
}

int main (void){


	//sleep(1);
	int checkRestart = 0;
	char buffer[BUFSIZ];	// Data buffer for communications
	int server_socket, client_socket;
	int client_len;
	struct sockaddr_in client_addr, server_addr;
	int len;
	char tbuffer[3] = "\0";//Buffer used to check if that data in the serial port

	/*  Linux RS-232  */
	int mainfd = 0;
	struct termios options;

    int StringLength = 0;
    char *serialOut;// pointer used to create buffer including the testring with flags
	
	int n;//used for error checking 
	int checkReturn = 0;//used to check with data is in the serial port
    

    

	        //Serial Settings
		mainfd =openPort();
		fcntl(mainfd,F_SETFL,FNDELAY);

		tcgetattr(mainfd,&options);
		cfsetispeed(&options,B9600);
		cfsetospeed(&options,B9600);

		options.c_cflag |= (CLOCAL|CREAD);
		options.c_cflag &= ~PARENB; /* Mask the character size to 8 bits, no parity */
		options.c_cflag &= ~CSTOPB;/* One stop bit*/
		options.c_cflag &= ~CSIZE;
		options.c_cflag |=  CS8;/* Select 8 data bits */
		options.c_cflag &= ~CRTSCTS;//Disable hardware control


		options.c_lflag &= ~(ICANON | ECHO | ISIG);/* Enable data to be processed as raw input */
		tcsetattr(mainfd, TCSANOW, &options);/* Set the new options for the port */
		int i =0;

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
          	memset(tbuffer, '\0', sizeof(tbuffer));
			read (client_socket, buffer, BUFSIZ);

			serialOut = (char*)calloc(strlen(buffer) + 2,sizeof(char));	

			 //put the startflag in begining of the string
			strncpy(serialOut, STARTFLAG, 1);
       		 //put the msg in middle
			strcat(serialOut, buffer);
       		 //put endflag after the msg
			strncat(serialOut, ENDFLAG, 1);
			write(mainfd,serialOut,strlen(serialOut));
			sleep(0.5);

	  		printf("Message from client: %s ... %s\n", buffer, serialOut);
	  		if (strcmp(buffer, "quit") == 0) {
	    		printf("Client has requested to quit, exiting application\n");
	    		break;
	  		}
	  		else if(strcmp(buffer, "") == 0){
	  			printf("Client has been disconnected. Restarting Application...\n");
	  			checkRestart = 1;
	  			close (client_socket);
	  			close (server_socket);
	  			return main();
				//free(serialOut);
	  			break;
	  		}

	  		// write data to client 
	  		write (client_socket, buffer, strlen(buffer));
	  		if (strcmp(buffer, "quit") == 0) {
	    		printf("Exiting application\n");
	    		break;
	  		}
	  		else if(strcmp(buffer, "")== 0){
	  			printf("Exiting Due to Disconnections.\n");
	  			break;
	  		}

	  		while(read(mainfd,tbuffer, sizeof(tbuffer)) != "\0"){	// Arish Heartbeat
	  			write(mainfd, "k", 1);
	  		}

	  		memset(serialOut, 0, sizeof(serialOut));
		}

	free(serialOut);
	close (client_socket);
	 
	return 0;
}
