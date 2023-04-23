
// I N C L U D E S ///////////////////////////////////////////////////////////

#include <dos.h>
#include <bios.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <iostream.h>

// D E F I N E S  ////////////////////////////////////////////////////////////

// mouse sub-function calls

#define MOUSE_INT                0x33 //mouse interrupt number
#define MOUSE_RESET              0x00 // reset the mouse
#define MOUSE_SHOW               0x01 // show the mouse
#define MOUSE_HIDE               0x02 // hide the mouse
#define MOUSE_BUTT_POS           0x03 // get buttons and postion
#define MOUSE_SET_SENSITIVITY    0x1A // set the sensitivity of mouse 0-100
#define MOUSE_MOTION_REL         0x0B // query motion counters to compute
												  // relative motion

// defines to make reading buttons easier

#define MOUSE_LEFT_BUTTON        0x01 // left button mask
#define MOUSE_RIGHT_BUTTON       0x02 // right button mask
#define MOUSE_CENTER_BUTTON      0x04 // center button mask


// G L O B A L S  ////////////////////////////////////////////////////////////



// F U N C T I O N S /////////////////////////////////////////////////////////


int Squeeze_Mouse(int command, int *x, int *y,int *buttons)
{
// mouse interface, we'll use int86 instead of inline asm...Why? No real reason.
// what function is caller requesting?

union REGS inregs, outregs;

switch(command)
		{

		case MOUSE_RESET:
			  {

			  inregs.x.ax = 0x00; // subfunction 0: reset
			  int86(MOUSE_INT, &inregs, &outregs);
			  *buttons = outregs.x.bx; // return number of buttons
			  return(outregs.x.ax);    // return overall success/failure

			  };

		case MOUSE_SHOW:
			  {
			  // this function increments the internal show mouse counter.
			  // when it is equal to 0 then the mouse will be displayed.

			  inregs.x.ax = 0x01; // subfunction 1: increment show flag
			  int86(MOUSE_INT, &inregs, &outregs);

			  return(1);

			  };

		case MOUSE_HIDE:
			  {
			  // this function decrements the internal show mouse counter.
			  // when it is equal to -1 then the mouse will be hidden.

			  inregs.x.ax = 0x02; // subfunction 2: decrement show flag
			  int86(MOUSE_INT, &inregs, &outregs);

			  return(1);

			  };

		case MOUSE_BUTT_POS:
			  {
			  // this functions gets the buttons and returns the absolute mouse
			  // positions in the vars x,y, and buttons, respectively

			  inregs.x.ax = 0x03; // subfunction 3: get position and buttons
			  int86(MOUSE_INT, &inregs, &outregs);

			  // extract the info and send back to caller via pointers
			  *x       = outregs.x.cx;
			  *y       = outregs.x.dx;
			  *buttons = outregs.x.bx;

			  return(1);

			  };

		case MOUSE_MOTION_REL:
			  {

			  // this functions gets the relative mouse motions from the last
			  // call and puts them in the vars x,y respectively

			  inregs.x.ax = 0x03; // subfunction 11: get relative motion
			  int86(MOUSE_INT, &inregs, &outregs);

			  // extract the info and send back to caller via pointers
			  *x       = outregs.x.cx;
			  *y       = outregs.x.dx;

			  return(1);

			  };

		case MOUSE_SET_SENSITIVITY:
			  {
			  // this function sets the overall "sensitivity" of the mouse.
			  // each axis can have a sensitivity from 1-100.  So the caller
			  // should put 1-100 in both "x" and "y" before calling/
			  // also "buttons" is used to send in the doublespeed value which
			  // ranges from 1-100 also.

			  inregs.x.bx = *x;
			  inregs.x.cx = *y;
			  inregs.x.dx = *buttons;

			  inregs.x.ax = 0x1A; // subfunction 26: set sensitivity
			  int86(MOUSE_INT, &inregs, &outregs);

			  return(1);

			  };

		default:break;

		} // end switch
return(0);

} // end Squeze_Mouse

//////////////////////////////////////////////////////////////////////////////
/*
void main(void)
{

int x,y,buttons,num_buttons;
int color=1;
int* sprite1 = (int*)malloc(SP_HEIGHT*SP_WIDTH*sizeof(int));
int* background = (int*)malloc(SP_HEIGHT*SP_WIDTH*sizeof(int));


// put the computer into graphics mode

Set_Video_Mode(VGA256); //  640x480 in 16 colors

Show_Bin("c:\\windows\\desktop\\gforce\\bin\\walkfram.bin");
Grab_Sp(0, 0, sprite1);

// initialize mouse

Squeeze_Mouse(MOUSE_RESET,NULL,NULL,&num_buttons);

// show the mouse

Show_Bin("c:\\windows\\desktop\\gforce\\bin\\bg.bin");
//Squeeze_Mouse(MOUSE_SHOW,NULL,NULL,NULL);

while(!kbhit())
	  {
	  Show_Sp(.5*x,y,sprite1);
	  Squeeze_Mouse(MOUSE_BUTT_POS,&x,&y,&buttons);
	  gotoxy(1,1);
	  cout << x << "," << y << "    ";
	  // video easel

	  if (buttons==1)
		  {
		  Pix(.5*x-1,y-2,color);
		  Pix(.5*x,y-2,color);
		  Pix(.5*x-1,y-1,color);
		  Pix(.5*x,y-1,color);
		  if (x < 16);
			{
			}
		 } // end if draw on

	  if (buttons==2)
		  {
		  if (++color>15) color=0;

		  // wait for mouse release

		  while(buttons==2)
				 {
				 Squeeze_Mouse(MOUSE_BUTT_POS,&x,&y,&buttons);
				 } // end while

		  } // end if draw on

	  } // end while

// place the computer back into text mode

Set_Video_Mode(TEXT_MODE);

} // end main

*/
