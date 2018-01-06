/*
 * client.c
 *
 * This is a sample internet client application that will talk
 * to the server server.c via port 5000
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
#define JOY_DIR "/dev/input/js0"	// Directory path of joystick controls
#define PI 3.14159265




int joyStick(int *axis, char *button, int joy_fd){

	
	//memset(axis, 0, sizeof(axis));

	

	struct js_event js;
	
	fcntl( joy_fd, F_SETFL, O_NONBLOCK );	/* use non-blocking mode */
	double val = 180.0 / PI;
	//int angle = 0;

	while( 1 ) 	/* infinite loop */
	{
			/* read the joystick state */
		read(joy_fd, &js, sizeof(struct js_event));
		
			/* see what to do with the event */
		switch (js.type & ~JS_EVENT_INIT)
		{
			case JS_EVENT_AXIS:
				axis   [ js.number ] = js.value;
				double ret = atan2((double)axis[2], (double)axis[3]) * val;
				printf("\n angle: %f axis[2]: %d axis[3]: %d", ret,axis[2],axis[3]);
				if((axis[0] || axis[1]) != 0){	
					if (axis[1] == -32767){
						//printf("UP");
						return 1;
					}

					else if (axis[1] == 32767){
						//printf("DOWN");
						return 2;
					}

					else if (axis[0] == -32767){
						//printf("LEFT");
						return 3;
					}

					else if (axis[0] == 32767){
						//printf("RIGHT");
						return 4;
					}
					else 
						return 7;
				}

				// angles for stepper motor
				else if((axis[2] || axis[3]) != 0){
					if((ret > 90) && (ret <= 95)){
						printf("10 Degrees \n");
						return 8;//break;
					}
					else if((ret > 100) && (ret <= 120)){
						printf("30 Degrees \n");
						return 9;//break;
					}
					else if((ret > 120) && (ret <= 150)){
						printf("50 Degrees \n");
						return 10;//break;
					}
					else if((ret > 150) && (ret <= 179)){
						printf("70 Degrees \n");
						return 11;//break;
					}
					else if((ret > 157.5) && (ret <= 180.54)){
						printf("90 Degrees \n");
						return 12;//break;
					}
					else if( ret < -157.5 && ret >= -178.818127){
						printf("90 Degrees \n");
						return 13;//break;
					}
					else if(ret < -100 && ret >= -120){
						printf("150 Degrees \n");
						return 16;
						//break;
					}
					else if(ret < -120 && ret >= -150){
						printf("130 Degrees \n");
						return 15;
						//break;
					}
					else if(ret < -150 && ret >= -178){
						printf("110 Degrees \n");
						return 14;
						//break;
					}
					else if(ret < -90 && ret >= -95){
						printf("170 Degrees \n");
						return 17;	
						//break;	
					}
				}
				break;

			case JS_EVENT_BUTTON:
				button [ js.number ] = js.value;
				if (button[4] == 1){
					//printf("tiltUp");
					return 5;
				}
				else if (button[6] == 1){
					//printf("tiltDown");
					return 6;
				}
				else if (button[2] == 1){
					return 0;
				}

				else if (button[0] == 1){
					return 18;
				}
				else if (button[1] == 1){
					return 19;
				}
				else if (button[11] == 1){
					return 12;
				}
				break;
		}

		
	}
	//return angle;

}



/****************************************************************************************/
int main (int argc, char *argv[])
{

	int *axis;
	char *button=NULL; 
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


	int num_of_axis=0, num_of_buttons=0;
	int joy_fd;

	/* check for proper device file open */
	if( ( joy_fd = open( JOY_DIR , O_RDONLY)) == -1 )
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
	//ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );


	axis = (int *) calloc( num_of_axis, sizeof( int ) );
	button = (char *) calloc( num_of_buttons, sizeof( char ) );	
	
	/* Get message from client and send to server */
	while(1) {
		memset(buffer, '\0', sizeof(buffer));
		memset(axis, 0, sizeof(int));
		memset(button, '\0', sizeof(char));
		sleep(0.1);		// delay is required between each function call
						// used to be before function is called

		  //printf ("Enter a message to send to the server (type 'quit' to exit): ");
		//fflush (stdout); 
		  //sleep(0.1);
		int js = joyStick((int*)axis, (char*)button, joy_fd);

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
		  	strncpy(buffer, "CS", sizeof(buffer));		//camera stream
		}
		else if(js == 19){
		  	strncpy(buffer, "CC", sizeof(buffer));		//camera capture
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
			printf("buffer: %s", buffer);
		    printf("\nExiting application\n");
		    break;
		}

	}
	free(axis);
	free(button);
	//close( joy_fd );	
	close (client_socket);
	close( joy_fd );

	return 0;
}	/* end main */
