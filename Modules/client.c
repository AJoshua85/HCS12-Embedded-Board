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
#include <sys/ioctl.h>
#include <linux/joystick.h>
#include <math.h>
#include <fcntl.h>

#define PORT 5550
#define JOY_DEV "/dev/input/js0"	// Directory path of joystick controls
#define PI 3.14159265





int joyStick(){

	int joy_fd, *axis=NULL, num_of_axis=0, num_of_buttons=0, x;
	char *button=NULL, name_of_joystick[80];
	struct js_event js;

		/* check for proper device file open */
	if( ( joy_fd = open( JOY_DEV , O_RDONLY)) == -1 )
	{
		printf( "Couldn't open joystick\n" );
		return -1;
	}


/* 	ioctl () - control device
			 - manipulates the underlying device paramets of special files
			   for example, the special joystick files that are used to read
			   the input values of the joystick
*/	

	ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
	ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
	ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );

	axis = (int *) calloc( num_of_axis, sizeof( int ) );
	button = (char *) calloc( num_of_buttons, sizeof( char ) );	
/*
	printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
		, name_of_joystick
		, num_of_axis
		, num_of_buttons );
*/
	fcntl( joy_fd, F_SETFL, O_NONBLOCK );	/* use non-blocking mode */

	while( 1 ) 	/* infinite loop */
	{

			/* read the joystick state */
		read(joy_fd, &js, sizeof(struct js_event));
		
			/* see what to do with the event */
		switch (js.type & ~JS_EVENT_INIT)
		{
			case JS_EVENT_AXIS:
				axis   [ js.number ] = js.value;
				if (axis[1] == -32767){
					//printf("UP");
					close( joy_fd );
					return 1;
				}

				else if (axis[1] == 32767){
					//printf("DOWN");
					close( joy_fd );
					return 2;
				}

				else if (axis[0] == -32767){
					//printf("LEFT");
					close( joy_fd );
					return 3;
				}

				else if (axis[0] == 32767){
					//printf("RIGHT");
					close( joy_fd );
					return 4;
				}


				//else if (axis[3] == )
				//else
					//printf("stop");
				break;
			case JS_EVENT_BUTTON:
				button [ js.number ] = js.value;
				if (button[4] == 1){
					//printf("tiltUp");
					close( joy_fd );
					return 5;
				}
				else if (button[6] == 1){
					close( joy_fd );
					//printf("tiltDown");
					return 6;
				}
				else if (button[2] == 1){
					close( joy_fd );
					return 0;
				}	
				else if (button[10] == 1){
					close( joy_fd );
					return 7;
				}
				else if (button[0] == 1){
					close( joy_fd );
					return 18;
				}
				else if (button[1] == 1){
					close( joy_fd );
					return 19;
				}

				break;

		}
		double val = 180 / PI;
		double ret = atan2((double)axis[2], (double)axis[3]) * val;
		if(ret > 90 && ret <= 95){
			//printf("10 Degrees ");
			return 8;
		}
		else if(ret > 95 && ret <= 112.5){
			//printf("30 Degrees ");
			return 9;
		}
		else if(ret > 112.5 && ret <= 135){
			//printf("50 Degrees ");
			return 10;
		}
		else if(ret > 135 && ret <= 157.5){
			//printf("70 Degrees ");
			return 11;
		}
		else if(ret > 157.5 && ret <= 180.54){
			//printf("90 Degrees ");
			return 12;
		}
		else if( ret < -157.5 && ret >= -178.818127){
			//printf("90 Degrees");
			return 13;
		}


		else if(ret < -95 && ret >= -112.5){
			//printf("150 Degrees ");
			return 16;
		}
		else if(ret < -112.5 && ret >= -135){
			//printf("130 Degrees ");
			return 15;
		}
		else if(ret < -135 && ret >= -157.5){
			//printf("110 Degrees ");
			return 14;
		}
		else if(ret < -90 && ret >= -95){
			//printf("170 Degrees ");
			return 17;		
		}
			
		// Carriage Return: repeats the loop until the user has pressed the Return key.	
		printf("  \r");	
		fflush(stdout);
	}

	close( joy_fd );	/* too bad we never get here */
	return 0;

}

int main (int argc, char *argv[])
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


	  //printf ("Enter a message to send to the server (type 'quit' to exit): ");
	  //fflush (stdout); 
	  sleep(1);
	  int js = joyStick();
	  if (js == 1){
	  	strncpy(buffer, "D08000700200", sizeof(buffer));		//forward DCM
	  }
	  else if(js == 2){
	  	strncpy(buffer, "D08000600200", sizeof(buffer));		//backward DCM
	  }
	  else if(js == 3){
	  	strncpy(buffer, "D08001000200", sizeof(buffer));		//left turn DCM
	  }
	  else if(js == 4){
	  	strncpy(buffer, "D08000900200", sizeof(buffer));		//right turn DCM
	  }
	  else if(js == 5){
	  	strncpy(buffer, "R00202000000", sizeof(buffer));		//neutral position for servo / tilit up
	  }
	  else if(js == 6){
	  	strncpy(buffer, "R00213000000", sizeof(buffer));		//tilt down
	  }
	  else if(js == 7){
	  	strncpy(buffer, "D08000800200", sizeof(buffer));		//STOP both DCM
	  }
	  else if(js == 8){
	  	strncpy(buffer, "S00001000000", sizeof(buffer));		//stepper 10 degrees
	  }
	  else if(js == 9){
	  	strncpy(buffer, "S00003000000", sizeof(buffer));		//stepper 30 degrees
	  }
	  else if(js == 10){
	  	strncpy(buffer, "S00005000000", sizeof(buffer));		//stepper 50 degrees
	  }
	  else if(js == 11){
	  	strncpy(buffer, "S00007000000", sizeof(buffer));		//stepper 70 degrees
	  }
	  else if(js == 12){
	  	strncpy(buffer, "S00009000000", sizeof(buffer));		//stepper 90 degrees
	  }
	  else if(js == 13){
	  	strncpy(buffer, "S00009000000", sizeof(buffer));		//stepper 90 degrees
	  }
	  else if(js == 14){
	  	strncpy(buffer, "S00011000000", sizeof(buffer));		//stepper 110 degrees
	  }
	  else if(js == 15){
	  	strncpy(buffer, "S00013000000", sizeof(buffer));		//stepper 130 degrees
	  }
	  else if(js == 16){
	  	strncpy(buffer, "S00015000000", sizeof(buffer));		//stepper 150 degrees
	  }
	  else if(js == 17){
	  	strncpy(buffer, "S00017000000", sizeof(buffer));		//stepper 170 degrees
	  }
	  else if(js == 18){
	  	strncpy(buffer, "cameraS", sizeof(buffer));		//camera stream
	  }
	   else if(js == 19){
	  	strncpy(buffer, "cameraC", sizeof(buffer));		//camera capture
	  }

	  else if(js == 0){
	  	strncpy(buffer, "quit\0", sizeof(buffer));
	  }

	  //fgets (buffer, sizeof (buffer), stdin); 
	  if (buffer[strlen (buffer) - 1] == '\n') {
	    buffer[strlen (buffer) - 1] = '\0'; 
          }
	  write (client_socket, buffer, strlen (buffer));
	  if (strcmp(buffer, "quit") == 0) {
	    printf("Exiting application\n");
	    break;
	  }


	  // cut this for continuous data stream
	  memset(buffer, 0, sizeof(buffer));
	  js = 0;
/*	  len = read (client_socket, buffer, sizeof (buffer));
	  printf ("Response from server: %s\n\n", buffer);
	  if (strcmp(buffer, "quit") == 0) {
	    printf("Server has decided to quit this connection, exiting application\n");
	    break;

	  }*/
	}
	//close( joy_fd );	
	close (client_socket);
	return 0;
}	/* end main */



