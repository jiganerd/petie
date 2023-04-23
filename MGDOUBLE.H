//28 graphics and sprite functions

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <math.h>
#include <io.h>
#include <Malloc.h>
#include <memory.h>

#define VGA256     0x13
#define SVGA       0x12
#define TEXT_MODE  0x03

#define PALETTE_MASK      0x3c6
#define PALETTE_REGISTER_RD 0x3c7
#define PALETTE_REGISTER_WR 0x3c8
#define PALETTE_DATA      0x3c9

#define VGA_INPUT_STATUS_1   0x3DA // vga status reg 1, bit 3 is the vsync
											  // when 1 - retrace in progress
											  // when 0 - no retrace
#define VGA_VSYNC_MASK 0x08        // masks off unwanted bit of status reg


#define SCREEN_WIDTH  (unsigned int)320
#define SCREEN_HEIGHT (unsigned int)200

#define SP_HEIGHT 37
#define SP_WIDTH  60

#define CHAR_HEIGHT 11
#define CHAR_WIDTH  10

#define TILE_HEIGHT 20
#define TILE_WIDTH  20
#define TILE_COUNT_X SCREEN_WIDTH/TILE_WIDTH
#define TILE_COUNT_Y SCREEN_HEIGHT/TILE_HEIGHT
#define MAP_HEIGHT 100
#define MAP_WIDTH  16

#define DEAD  0
#define ALIVE 1
#define DYING 2

#define RANDOM  0
#define PATTERN 1
#define CHASE   2

#define MAX_SPRITE_FRAMES 12

FILE* _fp;

typedef struct RGB_color_typ
		  {

		  unsigned char red;    // red   component of color 0-63
		  unsigned char green;  // green component of color 0-63
		  unsigned char blue;   // blue  component of color 0-63

		  } RGB_color, *RGB_color_ptr;

typedef struct bin_picture_typ
		  {
		  RGB_color palette[256];
		  char* buffer;
		  } bin_picture, *bin_picture_ptr;

typedef struct sprite_typ
		  {
		  float x, y;           // position of sprite
		  int width, height;  // dimensions of sprite in pixels
		  int h_width, h_height; //half of the width and height
		  float x_vel, y_vel;
		  int x_old, y_old;
		  int anim_clock;     // the animation clock
		  int anim_speed;     // the animation speed
		  int motion_speed;   // the motion speed
		  int motion_clock;   // the motion clock
		  int pattern_element;

		  char *frames[MAX_SPRITE_FRAMES]; // array of pointers to the images
		  int curr_frame;                      // current frame being displayed
		  int num_frames;                      // total number of frames
		  int state;                           // state of sprite, alive, dead...
		  int hits_taken;
		  int total_hp;
		  int flash;
		  int movement;
		  //char *background;                // whats under the sprite
		  } sprite, *sprite_ptr;

typedef struct dot_typ
		  {
		  int x, y;            // position of dot
		  int x_vel, y_vel;    // the velocity of the dot
		  char color;
		  int life_span;       // how long the dot has been alive
		  int state;           // state of dot, alive, dead...
		  } dot, *dot_ptr;

void Pix(int x, int y, unsigned char color);
void Pix_C(int x, int y, unsigned char color);
int Get_Pix(int x, int y);
void Line(int x1, int y1, int x2, int y2, unsigned char color);
void Set_Video_Mode(int mode);
void Set_Palette_Register(int index, RGB_color_ptr color);
void Get_Palette_Register(int index, RGB_color_ptr color);
void Show_Bin(char * filename, int pal);
void Grab_Sp(int x1, int y1, char* buffer);
void Show_Sp(int x1, int y1, char* buffer);
void Cls(void);
void Fade(void);
void Bin_Show_Buffer(bin_picture_ptr image);


unsigned char *video_buffer = (unsigned char *)0xA0000000L;
char * double_buffer = NULL;

//FUNCTIONS//////////////////////////////////////////////////////////////////
void Pix_C(int x, int y, unsigned char color)
{
double_buffer[((y<<8)+(y<<6))+x] = color;
}
/////////////////////////////////////////////////////////////////////////////
void Pix(int x, int y, unsigned char color)
{
	if (x > 319 || x < 0 || y > 199 || y < 0) return;
	double_buffer[((y<<8)+(y<<6))+x] = color;
}
/////////////////////////////////////////////////////////////////////////////
int Get_Pix(int x, int y)
{
return double_buffer[((y<<8)+(y<<6))+x];
}
/////////////////////////////////////////////////////////////////////////////
void Line(int x1, int y1, int x2, int y2, unsigned char color)
{
int i;

if ((x2 - x1) >  (y2 - y1))
{
	for (i = x1; i <= x2; i++)
	{
	double_buffer[((((i - x1) * (y2 - y1)/(x2 - x1) + y1)<<8)+(((i - x1) * (y2 - y1)/(x2 - x1) + y1)<<6))+i] = color;
	}
}
else
{
	for (i = y1; i <= y2; i++)
	{
	double_buffer[((i<<8)+(i<<6))+((i - y1) * (x2 - x1)/(y2 - y1) + x1, i)] = color;
	}
}

}
void Line_V(int x, int y1, int y2, unsigned char color)
{
int i;

for (i = y1; i <= y2; i++)
	{
	Pix(x, i, color);
	}

}
/////////////////////////////////////////////////////////////////////////////
void Set_Video_Mode(int mode)
{
	union REGS inregs, outregs;

	inregs.h.ah = 0;
	inregs.h.al = (unsigned char)mode;
	int86(0x10, &inregs, &outregs);
}
/////////////////////////////////////////////////////////////////////////////
void Set_Palette_Register(int index, RGB_color_ptr color)
{
_outp(PALETTE_MASK,0xff);
_outp(PALETTE_REGISTER_WR, index);
_outp(PALETTE_DATA,color->red);
_outp(PALETTE_DATA,color->green);
_outp(PALETTE_DATA,color->blue);
}
/////////////////////////////////////////////////////////////////////////////
void Get_Palette_Register(int index, RGB_color_ptr color)
{
_outp(PALETTE_MASK,0xff);
_outp(PALETTE_REGISTER_RD, index);
color->red   = _inp(PALETTE_DATA);
color->green = _inp(PALETTE_DATA);
color->blue  = _inp(PALETTE_DATA);
} // end Get_Palette_Color
/////////////////////////////////////////////////////////////////////////////
void Load_Bin(char * filename, bin_picture_ptr image, int pal)
{
int n, x, y;
unsigned char data;
RGB_color* color;
int index;

_fp = fopen(filename, "rb");

fseek(_fp,-768L,SEEK_END);
for (index=0; index<256; index++)
	 {
	 image->palette[index].red   = (getc(_fp));
	 image->palette[index].green = (getc(_fp));
	 image->palette[index].blue  = (getc(_fp));
	 } // end for index


// change the palette to newly loaded palette if commanded to do so

if (pal)
	{
	for (index=0; index<256; index++)
		 {
		 Set_Palette_Register(index,&image->palette[index]);
		 } // end for index
	} // end if change palette

fseek(_fp, 7L, SEEK_SET);

for (y = 0; y < SCREEN_HEIGHT; y++)
	  {
	  for (x = 0; x < SCREEN_WIDTH; x++)
			{
			data = getc(_fp);
			image->buffer[y * 320 + x] = data;
			if (feof(_fp)) break;
			}
	  }
fclose(_fp);
/////////////////////////////////////////////////////////////////////////////
}
void Cls(void)
{
memset(double_buffer, 0, SCREEN_WIDTH*SCREEN_HEIGHT);
}
/////////////////////////////////////////////////////////////////////////////
void Fade(void)
{
unsigned long int x;
int index,pal_reg;
RGB_color color,color_1,color_2,color_3;

for (index=0; index<40; index++)
	 {
	 for (pal_reg=0; pal_reg<256; pal_reg++)
		  {
		  Get_Palette_Register(pal_reg,(RGB_color_ptr)&color);
		  if (color.red   > 3) color.red-=2;
		  else
			  color.red = 0;

		  if (color.green > 3) color.green-=2;
		  else
			  color.green = 0;
		  if (color.blue  > 3) color.blue-=2;
		  else
			  color.blue = 0;
		  Set_Palette_Register(pal_reg,(RGB_color_ptr)&color);
		  } // end for pal_reg
	 for (x = 0; x < 1100000; x++);
	 } // end fade for
} // end Fade_Lights
/////////////////////////////////////////////////////////////////////////////
void Black_Out(void)
{
unsigned long int x;
int index,pal_reg;
RGB_color color;

for (pal_reg=0; pal_reg<256; pal_reg++)
  {
  color.red = color.green = color.blue = 0;
  Set_Palette_Register(pal_reg,(RGB_color_ptr)&color);
  } // end for pal_reg
} // end Black_Out
/////////////////////////////////////////////////////////////////////////////
void Fade_Up(RGB_color* palette)
{
unsigned long int x;
int index,pal_reg;
RGB_color color,color_1,color_2,color_3;

for (index=0; index<40; index++)
	 {
	 for (pal_reg=0; pal_reg<256; pal_reg++)
		  {
		  Get_Palette_Register(pal_reg,(RGB_color_ptr)&color);
		  if (color.red < palette[pal_reg].red - 3) color.red+=2;
		  else
			  color.red = palette[pal_reg].red;

		  if (color.green < palette[pal_reg].green - 3) color.green+=2;
		  else
			  color.green = palette[pal_reg].green;
		  if (color.blue  < palette[pal_reg].blue - 3) color.blue+=2;
		  else
			  color.blue = palette[pal_reg].blue;
		  Set_Palette_Register(pal_reg,(RGB_color_ptr)&color);
		  } // end for pal_reg
	 for (x = 0; x < 1100000; x++);
	 } // end fade for
} // end Fade_Lights
/////////////////////////////////////////////////////////////////////////////
void White(void)
{
unsigned long int x;
int index,pal_reg;
RGB_color color,color_1,color_2,color_3;

for (index=0; index<30; index++)
	 {
	 for (pal_reg=0; pal_reg<256; pal_reg++)
		  {
		  Get_Palette_Register(pal_reg,(RGB_color_ptr)&color);
		  if (color.red < 58) color.red+=3;
		  else
			  color.red = 63;

		  if (color.green < 58) color.green+=3;
		  else
			  color.green = 63;
		  if (color.blue  < 58) color.blue+=3;
		  else
			  color.blue = 58;
		  Set_Palette_Register(pal_reg,(RGB_color_ptr)&color);
		  } // end for pal_reg
	 for (x = 0; x < 500000; x++);
	 } // end fade for
} // end Fade_Lights
/////////////////////////////////////////////////////////////////////////////
void Bin_Init(bin_picture_ptr image)
{
if (!(image->buffer = (char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT + 1)))
	{
	printf("\ncouldn't allocate screen buffer");
	exit(1);
	}
} // end PCX_Init
/////////////////////////////////////////////////////////////////////////////
void Bin_Delete(bin_picture_ptr image)
{
free(image->buffer);
} // end PCX_Init
/////////////////////////////////////////////////////////////////////////////
void Bin_Show_Buffer(bin_picture_ptr image)
{
// just copy he pcx buffer into the video buffer

memcpy((char *)double_buffer,
			(char *)image->buffer,SCREEN_WIDTH*SCREEN_HEIGHT);

} // end PCX_Show_Picture
/////////////////////////////////////////////////////////////////////////////
void Grab(bin_picture_ptr image, sprite_ptr sprite, int sprite_frame,
		  int x1, int y1)
{
int x,y, index;
char *sprite_data;

// first allocate the memory for the sprite in the sprite structure
sprite->frames[sprite_frame] = (char *)malloc(sprite->width * sprite->height + 1);

sprite_data = sprite->frames[sprite_frame];

for (y = y1; y < (y1 + sprite->height); y++)
	 {
	 for (x = x1; x < (x1 + sprite->width); x++)
		  {
		  sprite_data[(y-y1)*(sprite->width) + (x - x1)] = image->buffer[y*320+x];
		  } // end for x
	 } // end for y
} // end PCX_Grap_Bitmap
/////////////////////////////////////////////////////////////////////////////
void Place(sprite_ptr sprite, sprite_ptr buffer_holder)
{
int x,y,sx,sy,sw,sh, start_x, start_y;
char *sprite_data;
sprite_data = buffer_holder->frames[sprite->curr_frame];
sx = sprite->x;
sy = sprite->y;
sw = sprite->width;
sh = sprite->height;
//start_x = 0;
//start_y = 0;
//int offset = (sy << 8) + (sy << 6) + sx;
//int work_offset = 0;

if (sx > SCREEN_WIDTH || sy > SCREEN_HEIGHT || (sx + sw) < 0 || (sy + sh) < 0) return;

if (sprite->flash == 1)
	{
	for (y = sy; y < (sy + sh); y++)
		{
		for (x = sx; x < (sx + sw); x++)
			{
			if (sprite_data[(y-sy)*(sw)+(x-sx)] != char(160))
			Pix(x, y, 206);
			} // end for x
		} // end for y
	}
else
	{
	for (y = sy; y < (sy + sh); y++)
		{
		for (x = sx; x < (sx + sw); x++)
			{
			if (sprite_data[(y-sy)*(sw)+(x-sx)] != char(160))
			Pix(x, y, sprite_data[(y-sy)*(sw)+(x-sx)]);
			} // end for x
		} // end for y
	}

} // end PCX_Grap_Bitmap
/////////////////////////////////////////////////////////////////////////////
void Show_Shadow(sprite_ptr sprite, sprite_ptr buffer_holder)
{
int x,y,sx,sy,sw,sh, start_x, start_y;
char *sprite_data;
sprite_data = buffer_holder->frames[sprite->curr_frame];
sx = ((sprite->x + .5*sprite->width) - 160)*.8 + 160 - .5*sprite->width;
sy = ((sprite->y + .5*sprite->height) - 100)*.8 + 100 - .5*sprite->height;
sw = sprite->width;
sh = sprite->height;
int draw = 1;

if (sx > SCREEN_WIDTH || sy > SCREEN_HEIGHT || (sx + sw) < 0 || (sy + sh) < 0) return;

if (sprite->width % 2 == 0)
	{
	for (y = sy; y < (sy + sh); y++)
		{
		draw *= -1;
		for (x = sx; x < (sx + sw); x++)
			{
			draw *= -1;
			if (sprite_data[(y-sy)*(sw)+(x-sx)] != char(160) && draw == 1)
				Pix(x, y, 0);
			} // end for x
		} // end for y
	}
else
	{
	for (y = sy; y < (sy + sh); y++)
		{
		for (x = sx; x < (sx + sw); x++)
			{
			draw *= -1;
			if (sprite_data[(y-sy)*(sw)+(x-sx)] != char(160) && draw == 1)
				Pix(x, y, 0);
			} // end for x
		} // end for y
	}

} // end PCX_Grap_Bitmap
/////////////////////////////////////////////////////////////////////////////
/*void Get_Bg(bin_picture_ptr image, sprite_ptr sprite)
{
int x,y,sx,sy,sw,sh;
char *sprite_data;

// first allocate the memory for the sprite in the sprite structure
sprite_data = sprite->background;

sx = sprite->x;
sy = sprite->y;
sw = sprite->width;
sh = sprite->height;

for (y = sy; y <= (sy + sh); y++)
	 {
	 for (x = sx; x <= (sx + sw); x++)
		  {
		  sprite_data[(y-sy)*(sw) + (x - sx)] = image->buffer[y*320+x];
		  } // end for x
	 } // end for y
} // end PCX_Grap_Bitmap
/////////////////////////////////////////////////////////////////////////////
void Erase_Sp(sprite_ptr sprite)
{
int x,y,sx,sy,sw,sh;
char *sprite_data;
sprite_data = sprite->background;
sx = sprite->x;
sy = sprite->y;
sw = sprite->width;
sh = sprite->height;

for (y = sy; y < (sy + sh); y++)
 {
	 for (x = sx; x < (sx + sw); x++)
		  {
		  Pix(x, y, sprite_data[(y-sy)*(sw)+(x-sx)]);
		  } // end for x
	 } // end for y
} // end PCX_Grap_Bitmap*/
/////////////////////////////////////////////////////////////////////////////
void Init_DB(void)
{
int x, y;

double_buffer = (char *)malloc(SCREEN_HEIGHT * SCREEN_WIDTH + 1);
memset(double_buffer, 0, SCREEN_WIDTH*SCREEN_HEIGHT);
}
/////////////////////////////////////////////////////////////////////////////
void Show_DB(void)
{
memcpy((char *)video_buffer, (char *)double_buffer, SCREEN_WIDTH*SCREEN_HEIGHT);
}
/////////////////////////////////////////////////////////////////////////////
void InitSprite(sprite_ptr sprite, int width, int height, int total_hp)
{
sprite->x_vel = 5;
sprite->y_vel = 5;
sprite->state = DEAD;
sprite->curr_frame = 0;
sprite->height = height;
sprite->width = width;
sprite->h_height = height/2;
sprite->h_width = width/2;
//sprite->background = (char*)farmalloc(sprite->width * sprite->height + 1);
sprite->anim_clock = 0;
sprite->anim_speed = 7;
sprite->motion_clock = 0;
sprite->motion_speed = 50;
sprite->pattern_element = 0;
sprite->hits_taken = 0;
sprite->total_hp = total_hp;
sprite->flash = 0;
sprite->movement = 0;
}
//////////////////////////////////////////////////////////////////////////////
void Blit_Char(int xc,int yc,char c, bin_picture_ptr font, int flash)
{
// this function uses the rom 8x8 character set to blit a character on the
// video screen, notice the trick used to extract bits out of each character
// byte that comprises a line
if (c == ' ') return;


int x, y;
char *data;
data = font->buffer + ((int(c) - 65)*CHAR_WIDTH);

if (flash)
	{
	for (y = yc; y < (yc + CHAR_HEIGHT); y++)
		 {
		 for (x = xc; x < (xc + CHAR_WIDTH); x++)
			  {
			  if (data[(y-yc)*SCREEN_WIDTH+(x-xc)] != char(160))
			  Pix(x, y, 181);
			  } // end for x
		 } // end for y
	} // end if flash
else
	{
	for (y = yc; y < (yc + CHAR_HEIGHT); y++)
		 {
		 for (x = xc; x < (xc + CHAR_WIDTH); x++)
			  {
			  if (data[(y-yc)*SCREEN_WIDTH+(x-xc)] != char(160))
			  Pix(x, y, data[(y-yc)*SCREEN_WIDTH+(x-xc)]);
			  } // end for x
		 } // end for y
	} // end if flash
} // end Blit_Char
/////////////////////////////////////////////////////////////////////////////
void Blit_String(int x,int y, char *string, bin_picture_ptr font, int flash)
{
// this function blits an entire string on the screen with fixed spacing
// between each character.  it calls blit_char.

int index;

for (index=0; string[index]!=0; index++)
	  {
	  Blit_Char(x+(index*10), y, string[index], font, flash);
	  } /* end while */

} /* end Blit_String */
/////////////////////////////////////////////////////////////////////////////
void Blit_Num(int xn,int yn, int num, bin_picture_ptr font, int flash)
{
// this function uses the rom 8x8 character set to blit a character on the
// video screen, notice the trick used to extract bits out of each character
// byte that comprises a line

int x, y;
char *data;
data = font->buffer + (SCREEN_WIDTH*CHAR_HEIGHT) + (num*CHAR_WIDTH);

if (flash)
	{
	for (y = yn; y < (yn + CHAR_HEIGHT); y++)
		 {
		 for (x = xn; x < (xn + CHAR_WIDTH); x++)
			  {
			  if (data[(y-yn)*SCREEN_WIDTH+(x-xn)] != char(160))
			  Pix(x, y, 181);
			  } // end for x
		 } // end for y
	} // end if flash
else
	{
	for (y = yn; y < (yn + CHAR_HEIGHT); y++)
		 {
		 for (x = xn; x < (xn + CHAR_WIDTH); x++)
			  {
			  if (data[(y-yn)*SCREEN_WIDTH+(x-xn)] != char(160))
			  Pix(x, y, data[(y-yn)*SCREEN_WIDTH+(x-xn)]);
			  } // end for x
		 } // end for y
	} // end if flash
} // end Blit_Char
/////////////////////////////////////////////////////////////////////////////
void Blit_Num_Mult(int xn,int yn, long num, bin_picture_ptr font, int flash)
{

int index, digit;

char string[10];
ltoa(num, string, 10);

for (index = 0; string[index] != 0; index++)
	{
	digit = string[index] - 48;
	Blit_Num(xn, yn, digit, font, flash);
	xn += CHAR_WIDTH;
	}
}
/////////////////////////////////////////////////////////////////////////////
void Grab_Tile(bin_picture_ptr image, char* buffer, int x1, int y1)
{
int x,y, index;

// first allocate the memory for the sprite in the sprite structure
//char* buffer = (char *)farmalloc(TILE_WIDTH * TILE_HEIGHT);

for (y = y1; y < (y1 + TILE_HEIGHT); y++)
	 {
	 for (x = x1; x < (x1 + TILE_WIDTH); x++)
		  {
		  buffer[(y-y1)*(TILE_WIDTH) + (x - x1)] = image->buffer[y*320+x];
		  } // end for x
	 } // end for y
} // end PCX_Grap_Bitmap
/////////////////////////////////////////////////////////////////////////////
void Blit_Tile(char *buffer_from, char *buffer_to, int x1, int y1)
{
int x, y;
int offset = (y1 << 8) + (y1 << 6) + x1;
int work_offset = 0;

for (y = 0; y < TILE_HEIGHT; y++)
	{
	memcpy(buffer_to+offset, buffer_from+work_offset, TILE_WIDTH);
	offset      += SCREEN_WIDTH;
	work_offset += TILE_WIDTH;
	} // end for y

}
/////////////////////////////////////////////////////////////////////////////
void Blit_Tile_Trans(char *buffer_from, char *buffer_to, int x1, int y1)
{
int x, y;
int offset = (y1 << 8) + (y1 << 6) + x1;
int work_offset = 0;

for (y = 0; y < TILE_HEIGHT; y++)
	{
	for (x = 0; x < TILE_WIDTH; x++)
		{
		if (buffer_from[work_offset+x] != char(160))
		buffer_to[offset+x] = buffer_from[work_offset+x];
		} // end for x
	offset      += SCREEN_WIDTH;
	work_offset += TILE_WIDTH;
	} // end for y

}
/////////////////////////////////////////////////////////////////////////////
void Delete_Sp(sprite_ptr sprite)
{
int index;

for (index = 0; index < MAX_SPRITE_FRAMES; index++)
	free(sprite->frames[index]);
//farfree(sprite->background);
}
/////////////////////////////////////////////////////////////////////////////
void Wait_for_Vsync(void)
{
// this function waits for the start of a vertical retrace, if a vertical
// retrace is in progress then it waits until the next one

while(_inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK)
	  {
	  // do nothing, vga is in retrace
	  } // end while

// now wait for vysnc and exit

while(!(_inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK))
	  {
	  // do nothing, wait for start of retrace
	  } // end while

// at this point a vertical retrace is occuring, so return back to caller

} // Wait_for_Vsync
