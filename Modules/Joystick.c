/* this is the linux 2.2.x way of handling joysticks. It allows an arbitrary
 * number of axis and buttons. It's event driven, and has full signed int
 * ranges of the axis (-32768 to 32767). It also lets you pull the joysticks
 * name. The only place this works of that I know of is in the linux 1.x 
 * joystick driver, which is included in the linux 2.2.x kernels


   	Author: Michael Luong,  with the assistance of:
   		https://www.kernel.org/doc/Documentation/input/joystick-api.txt
   	&	http://archives.seul.org/linuxgames/Aug-1999/msg00107.html
   	Date:	October 30, 2017
   	Purpose:
   		- Uses simple linux Joystick functions to read values from controller.
   		- Simple logic to return value necessary for robot movements
   		- Basic movements are controlled from return ranges from 1 to 9
   		- Servo Motor works from a toggling method from pressing an assigned button     	

 */

/* TO COMPILE ON LINUX
	gcc -o Joystick Joystick.c -lm
	BEFORE USING JOYSTICK:
	Make sure that the switch on the tail end of the controller is set ‘D’
	Test to make sure values are being received
	Ensure that “MODE” light is off
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

/* 	To search for correct input event from controller,
	ls /dev/input
	generally, it will be js0 OR event0
*/
#define JOY_DEV "/dev/input/js0"	// Directory path of joystick controls
#define PI 3.14159265

int main()
{
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
					printf("UP");
					//return 1;
				}

				else if (axis[1] == 32767){
					printf("DOWN");
					//return 2;
				}

				else if (axis[0] == -32767){
					printf("LEFT");
					//return 3;
				}

				else if (axis[0] == 32767){
					printf("RIGHT");
					//return 4;
				}


				//else if (axis[3] == )
				//else
					//printf("stop");
				break;
			case JS_EVENT_BUTTON:
				button [ js.number ] = js.value;
				if (button[4] == 1){
					printf("tiltUp");
					//return 6;
				}
				else if (button[6] == 1){
					printf("tiltDown");
					//return 6;
				}
				else if (button[0] == 1){
					printf("ToggleServo");
					//return 7;
				}	
				

				break;

		}

		/* PRINT THE RESULTS */

		// Left Joystick
		//printf( "X: %6d  Y: %6d  ", axis[0], axis[1] );
		

		/* Servo Motor Angles Using Right Joystick */
		double val = 180 / PI;
		double ret = atan2((double)axis[2], (double)axis[3]) * val;
		if(ret > 90 && ret <= 95){
			printf("10 Degrees ");
		}
		else if(ret > 95 && ret <= 112.5){
			printf("30 Degrees ");
		}
		else if(ret > 112.5 && ret <= 135){
			printf("50 Degrees ");
		}
		else if(ret > 135 && ret <= 157.5){
			printf("70 Degrees ");
		}
		else if(ret > 157.5 && ret <= 180.54){
			printf("90 Degrees ");
		}
		else if( ret < -157.5 && ret >= -178.818127){
			printf("90 Degrees");
		}


		else if(ret < -95 && ret >= -112.5){
			printf("150 Degrees ");
		}
		else if(ret < -112.5 && ret >= -135){
			printf("130 Degrees ");
		}
		else if(ret < -135 && ret >= -157.5){
			printf("110 Degrees ");
		}
		else if(ret < -90 && ret >= -95){
			printf("170 Degrees ");		
		}
		//printf("HERE LOOK HERE %1f degrees", ret);	
		//while ((axis[1] < "0") || (axis[1] > '0')){
			
		//}
/*
		//	Left trigger
		if( num_of_axis > 2 )
			printf("Z: %6d  ", axis[2] );
		
		// 	Right Joystick	
		if( num_of_axis > 3 )
			printf("R: %6d  ", axis[3] );
		//	Other buttons
		for( x=0 ; x<num_of_buttons ; ++x )
			printf("B%d: %d  ", x, button[x] );
		
*/
		// Carriage Return: repeats the loop until the user has pressed the Return key.	
		printf("  \r");	
		fflush(stdout);
	}

	close( joy_fd );	/* too bad we never get here */
	return 0;
}
