//forest game map editor

//INCLUDES////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <conio.h>
#include "mgdouble.h"
#include <key.h>
#include <mem.h>
#include <string.h>
#include <iostream.h>
//DEFINES/////////////////////////////////////////////////////////////////////
#define MAP_WIDTH			80
#define MAP_HEIGHT		40
#define MAX_TILES			47
//GLOBALS/////////////////////////////////////////////////////////////////////
int done = 0;
int mapon = 1;
int curr_x = 0,
	 curr_y = 0;

bin_picture tilegfx, font;

//map variables
char far* tiles[MAX_TILES];  //the bitmapped data for the tiled graphics
unsigned char map[MAP_WIDTH][MAP_HEIGHT][2]; //table for the level
int bg_seam_x;	//the num. of tiles from the left of the level
					//that the screen is currently on
int bg_seam_y; //the num. of tiles from the top of the level
					//that the screen is currently on
//end

//////////////////////////////////////////////////////////////////////////////
void main()
{
int i;
int x, y;

long delay;

Set_Video_Mode(VGA256);
Init_DB();


//load in font
Bin_Init(&font);
Load_Bin("font.bin", &font, 0);

//set up sprite variables
for (i = 0; i < MAX_TILES; i++)
	tiles[i] = (char far*)farmalloc(TILE_WIDTH*TILE_HEIGHT);

Bin_Init(&tilegfx);
Load_Bin("tiles.bin", &tilegfx, 1);
//first line
Grab_Tile(&tilegfx, tiles[0], 0, 0);
Grab_Tile(&tilegfx, tiles[1], 20, 0);
Grab_Tile(&tilegfx, tiles[2], 40, 0);
Grab_Tile(&tilegfx, tiles[3], 60, 0);
Grab_Tile(&tilegfx, tiles[4], 80, 0);
Grab_Tile(&tilegfx, tiles[5], 100, 0);
Grab_Tile(&tilegfx, tiles[6], 120, 0);
Grab_Tile(&tilegfx, tiles[7], 140, 0);
//second line
Grab_Tile(&tilegfx, tiles[8], 0, 20);
Grab_Tile(&tilegfx, tiles[9], 20, 20);
Grab_Tile(&tilegfx, tiles[10], 40, 20);
Grab_Tile(&tilegfx, tiles[11], 60, 20);
Grab_Tile(&tilegfx, tiles[12], 80, 20);
Grab_Tile(&tilegfx, tiles[13], 100, 20);
Grab_Tile(&tilegfx, tiles[14], 120, 20);
Grab_Tile(&tilegfx, tiles[15], 140, 20);
//third line
Grab_Tile(&tilegfx, tiles[16], 0, 40);
Grab_Tile(&tilegfx, tiles[17], 20, 40);
Grab_Tile(&tilegfx, tiles[18], 40, 40);
Grab_Tile(&tilegfx, tiles[19], 60, 40);
Grab_Tile(&tilegfx, tiles[20], 80, 40);
Grab_Tile(&tilegfx, tiles[21], 100, 40);
Grab_Tile(&tilegfx, tiles[22], 120, 40);
Grab_Tile(&tilegfx, tiles[23], 140, 40);
//fourth line
Grab_Tile(&tilegfx, tiles[24], 0, 60);
Grab_Tile(&tilegfx, tiles[25], 20, 60);
Grab_Tile(&tilegfx, tiles[26], 40, 60);
Grab_Tile(&tilegfx, tiles[27], 60, 60);
Grab_Tile(&tilegfx, tiles[28], 80, 60);
Grab_Tile(&tilegfx, tiles[29], 100, 60);
Grab_Tile(&tilegfx, tiles[30], 120, 60);
Grab_Tile(&tilegfx, tiles[31], 140, 60);
//fifth line
Grab_Tile(&tilegfx, tiles[32], 0, 80);
Grab_Tile(&tilegfx, tiles[33], 20, 80);
Grab_Tile(&tilegfx, tiles[34], 40, 80);
Grab_Tile(&tilegfx, tiles[35], 60, 80);
Grab_Tile(&tilegfx, tiles[36], 80, 80);
Grab_Tile(&tilegfx, tiles[37], 100, 80);
Grab_Tile(&tilegfx, tiles[38], 120, 80);
Grab_Tile(&tilegfx, tiles[39], 140, 80);
//sixth line
Grab_Tile(&tilegfx, tiles[40], 0, 100);
Grab_Tile(&tilegfx, tiles[41], 20, 100);
Grab_Tile(&tilegfx, tiles[42], 40, 100);
Grab_Tile(&tilegfx, tiles[43], 60, 100);
Grab_Tile(&tilegfx, tiles[44], 80, 100);
Grab_Tile(&tilegfx, tiles[45], 100, 100);
Grab_Tile(&tilegfx, tiles[46], 120, 100);
Bin_Delete(&tilegfx);

//load in map
FILE *fp;
char filename[9];

strcpy(filename, "lev1.map");
fp = fopen(filename, "rb");


for (y = 0; y < MAP_HEIGHT; y++)
	{
	for (x = 0; x < MAP_WIDTH; x++)
		{
		map[x][y][0] = getc(fp);
//		map[x][y][1] = EMPTY_TILE;
		}
	}
//for (y = 0; y < MAP_HEIGHT; y++)
//	for (x = 0; x < MAP_WIDTH; x++)
//		map[x][y][1] = getc(fp);

fclose(fp);
//end

while (done == 0)
{
switch(getch())
	{
	case (UP_ARROW_PRESSED): 	 if (curr_y > 0)
											{
											curr_y--;
											if (curr_y-bg_seam_y < 0) bg_seam_y--;
											}
										 break;
	case (DOWN_ARROW_PRESSED):  if (curr_y < MAP_HEIGHT)
											{
											curr_y++;
											if (curr_y-bg_seam_y > TILE_COUNT_Y-1) bg_seam_y++;
											}
										 break;
	case (LEFT_ARROW_PRESSED):  if (curr_x > 0)
											{
											curr_x--;
											if (curr_x-bg_seam_x < 0) bg_seam_x--;
											}
										 break;
	case (RIGHT_ARROW_PRESSED): if (curr_x < MAP_WIDTH)
											{
											curr_x++;
											if (curr_x-bg_seam_x > TILE_COUNT_X-1) bg_seam_x++;
											}
										 break;
	case ('q'): 			 			 done = 1;
										 break;
	case ('w'):  			 		 map[curr_x][curr_y][0] = 0;
										 break;
	case ('e'):  			 		 map[curr_x][curr_y][0] = 1;
											 break;
	case ('r'):  			 		 map[curr_x][curr_y][0] = 2;
										 break;
	case ('t'):  			 		 map[curr_x][curr_y][0] = 3;
										 break;
	case ('y'):  			 		 map[curr_x][curr_y][0] = 4;
										 break;
	case ('u'):  			 		 map[curr_x][curr_y][0] = 5;
										 break;
	case ('i'):  			 		 map[curr_x][curr_y][0] = 6;
										 break;
	case ('o'):  			 		 map[curr_x][curr_y][0] = 7;
										 break;
	case ('p'):  			 		 map[curr_x][curr_y][0] = 8;
										 break;
	case ('a'):  			 		 map[curr_x][curr_y][0] = 9;
										 break;
	case ('s'):  			 		 map[curr_x][curr_y][0] = 10;
										 break;
	case ('d'):  			 		 map[curr_x][curr_y][0] = 11;
										 break;
	case ('f'):  			 		 map[curr_x][curr_y][0] = 12;
										 break;
	case ('g'):  			 		 map[curr_x][curr_y][0] = 13;
										 break;
	case ('h'):  			 		 map[curr_x][curr_y][0] = 14;
										 break;
	case ('j'):  			 		 map[curr_x][curr_y][0] = 15;
										 break;
	case ('k'):  			 		 map[curr_x][curr_y][0] = 16;
										 break;
	case ('l'):  			 		 map[curr_x][curr_y][0] = 17;
										 break;
	case ('z'):  			 		 map[curr_x][curr_y][0] = 18;
										 break;
	case ('x'):  			 		 map[curr_x][curr_y][0] = 19;
										 break;
	case ('c'):  			 		 map[curr_x][curr_y][0] = 20;
										 break;
	case ('v'):  			 		 map[curr_x][curr_y][0] = 21;
										 break;
	case ('b'):  			 		 map[curr_x][curr_y][0] = 22;
										 break;
	case ('n'):  			 		 map[curr_x][curr_y][0] = 23;
										 break;
	case ('m'):  			 		 map[curr_x][curr_y][0] = 24;
										 break;
	case (';'):  			 		 map[curr_x][curr_y][0] = 25;
										 break;
	case (','):  			 		 map[curr_x][curr_y][0] = 26;
										 break;
	case ('!'):  			 		 map[curr_x][curr_y][0] = 27;
										 break;
	case ('@'):  			 		 map[curr_x][curr_y][0] = 28;
										 break;
	case ('#'):  			 		 map[curr_x][curr_y][0] = 29;
										 break;
	case ('$'):  			 		 map[curr_x][curr_y][0] = 30;
										 break;
	case ('%'):  			 		 map[curr_x][curr_y][0] = 31;
										 break;
	case ('^'):  			 		 map[curr_x][curr_y][0] = 32;
										 break;
	case ('&'):  			 		 map[curr_x][curr_y][0] = 33;
										 break;
	case ('*'):  			 		 map[curr_x][curr_y][0] = 34;
										 break;
	case ('('):  			 		 map[curr_x][curr_y][0] = 35;
										 break;
	case (')'):  			 		 map[curr_x][curr_y][0] = 36;
										 break;
	case ('_'):  			 		 map[curr_x][curr_y][0] = 37;
										 break;
	case ('+'):  			 		 map[curr_x][curr_y][0] = 38;
										 break;
	case ('W'):  			 		 map[curr_x][curr_y][0] = 39;
										 break;
	case ('E'):  			 		 map[curr_x][curr_y][0] = 40;
										 break;
	case ('R'):  			 		 map[curr_x][curr_y][0] = 41;
										 break;
	case ('T'):  			 		 map[curr_x][curr_y][0] = 42;
										 break;
	case ('Y'):  			 		 map[curr_x][curr_y][0] = 43;
										 break;
	case ('U'):  			 		 map[curr_x][curr_y][0] = 44;
										 break;
	case ('I'):  			 		 map[curr_x][curr_y][0] = 45;
										 break;
	case ('O'):  			 		 map[curr_x][curr_y][0] = 46;
										 break;
	}//end switch

for (y = 0; y < TILE_COUNT_Y; y++)
	{
	for (x = 0; x < TILE_COUNT_X; x++)
		{
		Blit_Tile(tiles[map[x+bg_seam_x][y+bg_seam_y][0]], double_buffer, x*TILE_WIDTH, y*TILE_HEIGHT);
		//Blit_Num_Mult((x-bg_seam_x)*TILE_WIDTH, (y-bg_seam_y)*TILE_HEIGHT,map[x][y][0],&font,0);
		}
	}

if (mapon==1)
{
for (y = 0; y < MAP_HEIGHT; y++)
	{
	for (x = 0; x < MAP_WIDTH; x++)
		{
		Pix(x,y,map[x][y][0]);
		}
	}
}

Line_V((curr_x-bg_seam_x)*TILE_WIDTH, (curr_y-bg_seam_y)*TILE_HEIGHT, (curr_y-bg_seam_y)*TILE_HEIGHT+TILE_HEIGHT, 18);
Line_V((curr_x-bg_seam_x)*TILE_WIDTH+TILE_WIDTH, (curr_y-bg_seam_y)*TILE_HEIGHT, (curr_y-bg_seam_y)*TILE_HEIGHT+TILE_HEIGHT, 18);
Line_H((curr_y-bg_seam_y)*TILE_HEIGHT, (curr_x-bg_seam_x)*TILE_WIDTH, (curr_x-bg_seam_x)*TILE_WIDTH+TILE_WIDTH, 18);
Line_H((curr_y-bg_seam_y)*TILE_HEIGHT+TILE_HEIGHT, (curr_x-bg_seam_x)*TILE_WIDTH, (curr_x-bg_seam_x)*TILE_WIDTH+TILE_WIDTH, 18);

//show graphics
//Cls();
Wait_For_Vsync();
Show_DB();
}

//cleanup
for (i = 0; i < MAX_TILES; i++)
	farfree(tiles[i]);
Free_DB();
Bin_Delete(&font);

Set_Video_Mode(TEXT_MODE);

//cout << "Save? (Y/N) ";

fp = fopen(filename, "rb+");
for (y = 0; y < MAP_HEIGHT; y++)
	{
	for (x = 0; x < MAP_WIDTH; x++)
		{
		putc(map[x][y][0], fp);
//		map[x][y][1] = EMPTY_TILE;
		}
	}
}//end main
